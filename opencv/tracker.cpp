#include <numeric>
#include "tracker.h"

using namespace Littai;



int TrackedItem::currentId = 0;


Tracker::Tracker(QQuickItem *parent)
    : Image(parent)
    , isOutputImage_(true)
    , templateThreshold_(0.2)
{
}


void Tracker::setInputImage(const QVariant &image)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        image.value<cv::Mat>().copyTo(inputImage_);
    }
    emit inputImageChanged();
}


QVariant Tracker::inputImage() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return QVariant::fromValue(inputImage_);
}


void Tracker::setTemplateImage(const QVariant &image)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        image.value<cv::Mat>().copyTo(templateImage_);
    }
    emit templateImageChanged();
}


QVariant Tracker::templateImage() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return QVariant::fromValue(templateImage_);
}


void Tracker::track()
{
    cv::Mat outputImage;
    cv::Mat grayInput;
    std::vector<TrackedItem> items;

    {
        std::lock_guard<std::mutex> lock(mutex_);

        if (inputImage_.empty() || templateImage_.empty()) {
            return;
        }
        outputImage = inputImage_.clone();
    }

    cv::cvtColor(outputImage, grayInput, cv::COLOR_BGR2GRAY);
    cv::threshold(grayInput, grayInput, 100, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    // 解像度の関係でサイズを半分にする必要あり？（要調査）
    cv::Mat grayTemplate;
    cv::cvtColor(templateImage_, grayTemplate, cv::COLOR_BGR2GRAY);
    cv::threshold(grayTemplate, grayTemplate, 100, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    cv::resize(grayTemplate, grayTemplate, cv::Size(), 0.5, 0.5, cv::INTER_LINEAR);

    const auto templateWidth  = grayTemplate.rows;
    const auto templateHeight = grayTemplate.cols;
    const auto templateScale  = (templateWidth + templateHeight) / 2;
    const cv::Point templateSize(templateWidth, templateHeight);

    cv::Mat result;
    cv::matchTemplate(grayInput, grayTemplate, result, cv::TM_CCOEFF_NORMED);

    // 閾値以内の Template Matching の結果を順番に見ていく
    double maxVal = 1;
    for (int maxTry = 0; maxTry < 10; ++maxTry) {
        double minVal;
        cv::Point minPos, maxPos;
        cv::minMaxLoc(result, &minVal, &maxVal, &minPos, &maxPos);

        // 閾値を下回ったら終了
        if (maxVal < templateThreshold_) {
            break;
        }

        // 外側のものは無視
        if (maxPos.x < 0 || maxPos.x + templateWidth  >= grayInput.cols ||
            maxPos.y < 0 || maxPos.y + templateHeight >= grayInput.rows) {
            continue;
        }

        // 認識したパターンを赤い四角で表示
        if (isOutputImage_) {
            cv::rectangle(
                outputImage,
                maxPos,
                maxPos + templateSize,
                CV_RGB(255, 0, 0),
                2);
        }

        // 認識した場所は黒く塗りつぶす（テンプレートマッチング結果）
        cv::rectangle(
            result,
            maxPos - templateSize * 0.5,
            maxPos + templateSize * 0.5,
            CV_RGB(0, 0, 0),
            -1);

        // 認識した場所を ROI で区切る
        auto roi = grayInput(cv::Rect(maxPos, maxPos + templateSize));
        std::vector<cv::Mat> contours;
        cv::findContours(roi, contours, cv::RETR_EXTERNAL, 2);
        auto moments = cv::moments(contours[0]);

        // 重心点（~ ランドルト環の中心）を求める
        // （実際は穴の空いてる位置から若干離れる位置に来る）
        if (moments.m00 == 0) continue;
        const int centerX = moments.m10 / moments.m00;
        const int centerY = moments.m01 / moments.m00;

        // ランドルト環の中心から放射状にレイを飛ばし、
        // 通り抜けたレイの平均角度を認識したアイテムの回転角とする
        std::vector<double> holeAngles;
        std::vector<double> radiuses;
        const int div = 100;

        for (int i = 0; i < div; ++i) {
            const auto angle = 2 * M_PI * i / div;

            bool isHit = false;

            // だんだんと半径を大きくしていく（レイを伸ばす）
            for (int r = 0; r < templateScale; r += 5) {
                int x = centerX + r * cos(angle);
                int y = centerY + r * sin(angle);
                if (x < 0 || y < 0 || x >= templateWidth || y >= templateHeight) {
                    break;
                }
                // ヒットしたら次へ
                if (roi.at<unsigned char>(y, x) > 0) {
                    isHit = true;
                    radiuses.push_back(r);
                    break;
                } else if (isOutputImage_) {
                    // レイを飛ばした場所は赤く塗っておく
                    outputImage.at<cv::Vec3b>(maxPos.y + y, maxPos.x + x) = cv::Vec3b(0, 0, 255);
                }
            }

            // ROI の端まで到達したレイの角度を登録しておく
            if (!isHit) {
                // 境界（0 - 360 付近）をまたがる場合は補正
                if (!holeAngles.empty() && abs(holeAngles.front() - angle) > M_PI) {
                    holeAngles.push_back(angle - 2 * M_PI);
                } else {
                    holeAngles.push_back(angle);
                }
            }
        }

        // 重心に○を描く
        if (isOutputImage_) {
            cv::circle(outputImage, maxPos + cv::Point(centerX, centerY), 3, CV_RGB(0, 255, 0), -1);
        }

        if (!holeAngles.empty()) {
            // 中心点（TODO: 重心位置でなく実際の中心にする）
            const auto center = maxPos + cv::Point(centerX, centerY);

            // 平均角
            const auto averageHoleAngle = std::accumulate(holeAngles.begin(), holeAngles.end(), 0.0) / holeAngles.size();

            // 平均半径
            double radius = -1.0;
            if (!radiuses.empty()) {
                radius = std::accumulate(radiuses.begin(), radiuses.end(), 0.0) / radiuses.size();
            }

            // トラッキング情報を登録
            TrackedItem item;
            item.x      = center.x;
            item.y      = center.y;
            item.width  = templateWidth;
            item.height = templateHeight;
            item.angle  = averageHoleAngle;
            item.radius = radius;
            items.push_back(item);

            // 認識した角度を示す矢印を描く
            if (isOutputImage_) {
                const auto dir = cv::Point(
                    static_cast<int>(templateScale / 2 * cos(averageHoleAngle)),
                    static_cast<int>(templateScale / 2 * sin(averageHoleAngle)));
                cv::arrowedLine(outputImage, center, center + dir, CV_RGB(0, 255, 0), 1);
            }
        }
    }

    // トラッキング情報を更新する
    updateItems(items);

    // 描画
    if (isOutputImage_) {
        setImage(outputImage);
        update();
    }
}


void Tracker::updateItems(const std::vector<TrackedItem>& currentItems)
{
    std::vector<TrackedItem> newItems;

    // 既に登録されているアイテムと比較して近い位置なら
    // その情報を引き継ぐ
    for (const auto& currentItem : currentItems) {
        bool isExists = false;
        for (auto&& item : items_) {
            const auto dx = currentItem.x - item.x;
            const auto dy = currentItem.y - item.y;
            const auto distance = sqrt(dx * dx + dy * dy);
            if (distance < 50.0) {
                item.x       = currentItem.x;
                item.y       = currentItem.y;
                item.width   = currentItem.width;
                item.height  = currentItem.height;
                item.radius  = currentItem.radius;
                item.angle   = currentItem.angle;
                item.checked = true;
                isExists = true;
                break;
            }
        }
        if (!isExists) {
            newItems.push_back(currentItem);
        }
    }

    // 認識されなかったアイテムは削除
    // 認識されたアイテムはフレームカウントを増加
    for (auto it = items_.begin(); it != items_.end();) {
        auto& item = *it;
        if (item.checked) {
            item.checked = false;
            ++item.frameCount;
        } else {
            it = items_.erase(it);
            continue;
        }
        ++it;
    }

    // 新規アイテムを追加
    for (auto&& item : newItems) {
        item.id = TrackedItem::GetId();
        items_.push_back(item);
    }

    emit itemsChanged();
}


QVariantList Tracker::items()
{
    QVariantList items;

    for (auto&& item : items_) {
        QVariantMap o;
        o.insert("id",         item.id);
        o.insert("x",          item.x);
        o.insert("y",          item.y);
        o.insert("width",      item.width);
        o.insert("height",     item.height);
        o.insert("radius",     item.radius);
        o.insert("angle",      item.angle);
        o.insert("frameCount", item.frameCount);
        items.append(o);
    }

    return items;
}
