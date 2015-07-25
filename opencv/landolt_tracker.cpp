#define _USE_MATH_DEFINES
#include <numeric>
#include "landolt_tracker.h"

using namespace Littai;



int TrackedItem::currentId = 0;


LandoltTracker::LandoltTracker(QQuickItem *parent)
    : Image(parent)
    , isFinished_(false)
    , isOutputImage_(true)
    , isImageUpdated_(false)
    , contrastThreshold_(100)
    , touchContrastThreshold_(100)
    , templateThreshold_(0.2)
    , fps_(30)
{
    thread_ = std::thread([&] {
        using namespace std::chrono;
        while (!isFinished_) {
            const auto t1 = high_resolution_clock::now();
            track();
            const auto t2 = high_resolution_clock::now();
            const auto dt = t2 - t1;
            const auto waitTime = microseconds(1000000 / fps_) - dt;
            if (waitTime > microseconds::zero()) {
                std::this_thread::sleep_for(waitTime);
            }
        }
    });
}


LandoltTracker::~LandoltTracker()
{
    isFinished_ = true;
    if (thread_.joinable()) {
        thread_.join();
    }
}


void LandoltTracker::setInputImage(const QVariant &image)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        image.value<cv::Mat>().copyTo(inputImage_);
        isImageUpdated_ = true;
    }
    emit inputImageChanged();
}


QVariant LandoltTracker::inputImage() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return QVariant::fromValue(inputImage_);
}


void LandoltTracker::setTemplateImage(const QVariant &image)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        image.value<cv::Mat>().copyTo(templateImage_);
    }
    emit templateImageChanged();
}


QVariant LandoltTracker::templateImage() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return QVariant::fromValue(templateImage_);
}


void LandoltTracker::track()
{
    if (!isImageUpdated_) return;

    cv::Mat grayInput, grayInputRaw, outputImage;
    {
        std::lock_guard<std::mutex> lock(mutex_);

        if (inputImage_.empty() || templateImage_.empty()) {
            return;
        }
        cv::cvtColor(inputImage_, grayInput, cv::COLOR_BGR2GRAY);
    }
    grayInputRaw = grayInput.clone();

    // フィルタ
    preProcess(grayInput);

    // 出力画像
    cv::cvtColor(grayInput, outputImage, cv::COLOR_GRAY2BGR);

    // ランドルト環検出
    detectLandolt(outputImage, grayInput);

    // タッチ検出
    detectLandoltTouch(outputImage, grayInputRaw);

    if (isOutputImage_) {
        setImage(outputImage, false);
    }

    isImageUpdated_ = false;
}


void LandoltTracker::preProcess(cv::Mat &image)
{
    cv::threshold(image, image, contrastThreshold_, 255, cv::THRESH_BINARY);
    cv::medianBlur(image, image, 5);
}


void LandoltTracker::detectLandolt(cv::Mat &outputImage, cv::Mat &inputImage)
{
    // 解像度の関係でサイズを半分にする必要あり？（要調査）
    const double shrinkScale = 0.3;
    cv::Mat grayInputSmall, grayTemplate;
    cv::cvtColor(templateImage_, grayTemplate, cv::COLOR_BGR2GRAY);
    cv::resize(grayTemplate, grayTemplate, cv::Size(), shrinkScale / 2, shrinkScale / 2, cv::INTER_LINEAR);
    cv::resize(inputImage, grayInputSmall, cv::Size(), shrinkScale, shrinkScale, cv::INTER_LINEAR);

    const auto templateWidth  = grayTemplate.rows / shrinkScale;
    const auto templateHeight = grayTemplate.cols / shrinkScale;
    const auto templateScale  = (templateWidth + templateHeight) / 2;
    const cv::Point templateSize(templateWidth, templateHeight);

    cv::Mat result;
    cv::matchTemplate(grayInputSmall, grayTemplate, result, cv::TM_CCOEFF_NORMED);

    // 閾値以内の Template Matching の結果を順番に見ていく
    std::vector<TrackedItem> items;
    double maxVal = 1;
    for (int maxTry = 0; maxTry < 5; ++maxTry) {
        double minVal;
        cv::Point minPos, maxPos;
        cv::minMaxLoc(result, &minVal, &maxVal, &minPos, &maxPos);
        maxPos *= 1 / shrinkScale;

        // 閾値を下回ったら終了
        if (maxVal < templateThreshold_) {
            break;
        }

        // 外側のものは無視
        if (maxPos.x < 0 || maxPos.x + templateWidth  >= inputImage.cols ||
            maxPos.y < 0 || maxPos.y + templateHeight >= inputImage.rows) {
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
        cv::circle(
            result,
            maxPos * shrinkScale,
            templateScale * shrinkScale * 0.75,
            CV_RGB(0, 0, 0),
            -1);

        // 認識した場所を ROI で区切る
        auto roi = inputImage(cv::Rect(maxPos, maxPos + templateSize));

        // 重心点（~ ランドルト環の中心）を求める
        // （実際は穴の空いてる位置から若干離れる位置に来る）
        /*
        std::vector<cv::Mat> contours;
        cv::findContours(roi, contours, cv::RETR_EXTERNAL, 2);
        auto moments = cv::moments(contours[0]);
        if (moments.m00 == 0) continue;
        const int centerX = moments.m10 / moments.m00;
        const int centerY = moments.m01 / moments.m00;
        */
        const int centerX = templateSize.x / 2;
        const int centerY = templateSize.y / 2;

        // ランドルト環の中心から放射状にレイを飛ばし、
        // 通り抜けたレイの平均角度を認識したアイテムの回転角とする
        std::vector<double> holeAngles;
        std::vector<double> radiuses;
        const int div = 120;

        for (int i = 0; i < div; ++i) {
            const auto angle = 2 * M_PI * i / div;

            bool isHit = false;

            // だんだんと半径を大きくしていく（レイを伸ばす）
            for (int r = 0; r < templateScale; r += 3) {
                int x = centerX + r * cos(angle);
                int y = centerY + r * sin(angle);
                if (x < 0 || y < 0 || x >= roi.cols || y >= roi.rows) {
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
                if (!holeAngles.empty() && std::abs(holeAngles.front() - angle) > M_PI) {
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

            // 認識した角度を示す矢印を描く
            if (isOutputImage_) {
                const auto dir = cv::Point(
                    static_cast<int>(templateScale / 2 * cos(averageHoleAngle)),
                    static_cast<int>(templateScale / 2 * sin(averageHoleAngle)));
                cv::arrowedLine(outputImage, center, center + dir, CV_RGB(0, 255, 0), 1);
            }

            // トラッキング情報を登録
            TrackedItem item;
            item.x      = center.x;
            item.y      = center.y;
            item.width  = templateWidth;
            item.height = templateHeight;
            item.angle  = averageHoleAngle;
            item.radius = radius;
            item.image  = outputImage(cv::Rect(
                center - cv::Point(templateScale / 2, templateScale / 2),
                center + cv::Point(templateScale / 2, templateScale / 2))).clone();
            items.push_back(item);
        }
    }

    // トラッキング情報を更新する
    updateItems(items);
}


void LandoltTracker::updateItems(const std::vector<TrackedItem>& currentItems)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);

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
                    item.image   = currentItem.image;
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
    }

    emit itemsChanged();
}


void LandoltTracker::detectLandoltTouch(cv::Mat &outputImage, cv::Mat &inputImage)
{
    for (auto&& item : items_) {
        cv::Mat roi = inputImage(cv::Rect(
            cv::Point(item.x - item.width / 2, item.y - item.height / 2),
            cv::Point(item.x + item.width / 2, item.y + item.height / 2))).clone();

        const auto r = item.radius * 0.6;
        int sum = 0;
        int n = 0;
        for (int x = 0; x < roi.cols; ++x) {
            const int cx = x - item.width / 2;
            for (int y = 0; y < roi.rows; ++y) {
                const int cy = y - item.height / 2;
                const int index = y * roi.step + x;
                if (cx * cx + cy * cy > r * r) {
                    roi.data[index] = 0;
                } else {
                    sum += roi.data[index];
                    ++n;
                }
            }
        }
        int average = sum / n;

        cv::Mat roiAverage(cv::Size(roi.cols, roi.rows), roi.type(), cv::Scalar(average, average, average));
        cv::subtract(roi, roiAverage, roi);
        cv::threshold(roi, roi, touchContrastThreshold_, 255, cv::THRESH_BINARY);
        cv::medianBlur(roi, roi, 5);

        double mx = 0, my = 0, total = 0;
        for (int x = 0; x < roi.cols; ++x) {
            const int cx = x - item.width / 2;
            for (int y = 0; y < roi.rows; ++y) {
                const int cy = y - item.height / 2;
                const int index = y * roi.step + x;
                if (cx * cx + cy * cy < r * r) {
                    mx += roi.data[index] * x;
                    my += roi.data[index] * y;
                    total += roi.data[index];
                }
            }
        }
        mx /= total;
        my /= total;
        cv::circle(roi, cv::Point(mx, my), 5, cv::Scalar(255, 255, 255), 1);
        cv::imshow("hoge", roi);
        cv::waitKey(1);
    }
}


QVariantList LandoltTracker::items()
{
    std::lock_guard<std::mutex> lock(mutex_);

    QVariantList items;
    if (inputImage_.empty()) return items;

    const int width  = inputImage_.rows;
    const int height = inputImage_.cols;

    for (auto&& item : items_) {
        QVariantMap o;
        o.insert("id",         item.id);
        o.insert("x",          2.0 * item.x / width - 1.0);
        o.insert("y",          1.0 - 2.0 * item.y / height);
        o.insert("width",      item.width / width);
        o.insert("height",     item.height / height);
        o.insert("radius",     item.radius / ((width + height) / 2));
        o.insert("angle",      item.angle);
        o.insert("frameCount", item.frameCount);
        o.insert("image",      QVariant::fromValue(item.image));
        items.append(o);
    }

    return items;
}
