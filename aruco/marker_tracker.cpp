#include <numeric>
#include <aruco.h>
#include <opencv2/opencv.hpp>
#include "marker_tracker.h"

using namespace Littai;


namespace
{
    template <class T>
    double len(const T& v)
    {
        return std::sqrt(v.x * v.x + v.y * v.y);
    }
}



MarkerTracker::MarkerTracker(QQuickItem *parent)
    : Image(parent)
    , contrastThreshold_(100)
{
}


void MarkerTracker::setInputImage(const QVariant &image)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        image.value<cv::Mat>().copyTo(inputImage_);
    }
    emit inputImageChanged();
}


QVariant MarkerTracker::inputImage() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return QVariant::fromValue(inputImage_);
}


void MarkerTracker::track()
{
    if (inputImage_.empty()) return;

    cv::Mat inputImage;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        inputImage = inputImage_.clone();
    }

    preProcess(inputImage);

    // ブロック検出
    cv::Mat gray;
    cv::cvtColor(inputImage, gray, cv::COLOR_BGR2GRAY);
    cv::medianBlur(gray, gray, 3);
    std::vector<std::vector<cv::Point>> contours;
    cv::threshold(gray, gray, 50, 255, cv::THRESH_OTSU);
    cv::findContours(gray, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_TC89_KCOS);

    auto markers = detectUsingAruco(inputImage);

    // フラグをオフにしておく
    for (auto&& marker : markers_) {
        marker.checked = false;
    }

    // 見つかったアイテムは情報を更新してフラグを立てる
    for (auto&& newMarker : markers) {
        bool isFound = false;
        for (auto&& marker : markers_) {
            if (newMarker.id == marker.id) {
                isFound = true;
                marker.update(newMarker);
                marker.checked = true;
                break;
            }
        }
        if (!isFound) {
            markers_.push_back(newMarker);
        }
    }

    // しばらく認識されなかったマーカは削除
    // 認識されたアイテムはフレームカウントを増加
    for (auto it = markers_.begin(); it != markers_.end();) {
        auto& marker = *it;
        if (marker.checked) {
            marker.frameCount++;
            if (marker.frameCount > 30) {
                // TODO: 検出イベントを送信
            }
            marker.lostCount = 0;
        } else {
            marker.lostCount++;
            if (marker.lostCount > 60) {
                it = markers_.erase(it);
                // TODO: ロストイベントを送信
                continue;
            }
        }
        ++it;
    }

    for (auto&& marker : markers_) {
        marker.print();
    }

    // マーカを囲む形状を認識
    if (contours.size() > 0 && markers_.size() > 0) {
        // 領域を大きい順に並べる
        std::sort(contours.begin(), contours.end(),
            [](const std::vector<cv::Point>& a, const std::vector<cv::Point>& b) {
                return cv::contourArea(a) > cv::contourArea(b);
            });

        // マーカを内包する領域を調べる
        for (auto&& marker : markers_) {
            for (int i = 0; i < contours.size(); ++i) {
                const auto& contour = contours[i];
                const cv::Point pt(marker.rawX, marker.rawY);
                if (cv::pointPolygonTest(contour, pt, 0) == 1) {
                    marker.contour = contour;
                    break;
                }
            }
        }
    }

    for (auto&& marker : markers_) {
        // 中心と領域を描画
        const cv::Point center(marker.rawX, marker.rawY);
        cv::circle(inputImage, center, marker.size / 2, cv::Scalar(0, 255, 0), 2);
        std::vector<std::vector<cv::Point>> contours = { marker.contour };
        cv::drawContours(inputImage, contours, 0, cv::Scalar(255, 0, 0), 3);

        std::vector<cv::Point> polygon;
        cv::approxPolyDP(marker.contour, polygon, 5, true);

        for (const auto& vertex : polygon) {
            const auto d = center - vertex;
            if (len(d) > 60) {
                cv::circle(inputImage, vertex, 4, cv::Scalar(0, 0, 255), -1);
            } else {
                cv::circle(inputImage, vertex, 2, cv::Scalar(255, 0, 255), -1);
            }
        }

        // 突端認識
        if (polygon.size() >= 4) {
            std::vector<cv::Point> edges;
            const int N = polygon.size();
            for (int i = 0; i < N; ++i) {
                const int i0 = i;
                const int i1 = ((i + 1) < polygon.size()) ? (i + 1) : (i + 1 - N);
                const int i2 = ((i + 2) < polygon.size()) ? (i + 2) : (i + 2 - N);
                const int i3 = ((i + 3) < polygon.size()) ? (i + 3) : (i + 3 - N);

                const auto v0 = polygon[i0];
                const auto v1 = polygon[i1];
                const auto v2 = polygon[i2];
                const auto v3 = polygon[i3];

                const auto s1 = v1 - v0;
                const auto s2 = v2 - v1;
                const auto s3 = v3 - v2;

                const auto center = (v0 + v1 + v2 + v3) * 0.25;
                const double ratio = 0.7;

                qDebug() << gray.cols << "  " << gray.rows;

                const bool isMiddleShort = len(s2) / len(s1) < ratio && len(s2) / len(s3) < ratio;
                const bool isOpposite    = s1.dot(s3) < -0.5;
                const bool isMidInner    = gray.at<unsigned char>(center.y, center.x) > 0;

                if (isMiddleShort && isOpposite && isMidInner) {
                    edges.push_back( (v1 + v2) * 0.5 );
                }
            }

            for (const auto& edge : edges) {
                cv::circle(inputImage, edge, 6, cv::Scalar(0, 255, 0), -1);
            }
        }
    }

    setImage(inputImage);
}


void MarkerTracker::preProcess(cv::Mat &inputImage)
{
    cv::Mat lut(1, 256, CV_8U);
    for (int i = 0; i < 256; ++i) {
        const auto val = std::pow(i, 2);
        lut.at<unsigned char>(i) = (i < contrastThreshold_) ?
            val / contrastThreshold_ :
            (256 - val / std::pow(256, 2));
    }
    cv::LUT(inputImage, lut, inputImage);

    imageCaches_.push_back(inputImage);
    if (imageCaches_.size() > 2) {
        imageCaches_.pop_front();
    }
    cv::Mat input(inputImage.rows, inputImage.cols, inputImage.type(), cv::Scalar(0));
    for (auto&& cache : imageCaches_) {
        input += cache * 1.0 / imageCaches_.size();
    }
    inputImage = input;
}


std::vector<TrackedMarker> MarkerTracker::detectUsingAruco(cv::Mat &inputImage)
{
    cv::Mat image;
    cv::cvtColor(inputImage, image, cv::COLOR_BGR2GRAY);
    cv::resize(image, image, cv::Size(), 1.5, 1.5, cv::INTER_LINEAR);

    // カメラパラメタのロード
    aruco::CameraParameters params;
    params.readFromXMLFile(cameraParamsFilePath_.toStdString());
    params.resize(image.size());

    // マーカを認識する
    aruco::MarkerDetector detector;
    std::vector<aruco::Marker> markers;
    const float markerSize = 0.03f;
    cv::cvtColor(inputImage, image, cv::COLOR_BGR2GRAY);
    detector.detect(image, markers, params, markerSize);

    // 結果を書き出す
    std::vector<TrackedMarker> result;
    for (auto&& marker : markers) {
        // marker.draw(inputImage, cv::Scalar(0, 0, 255), 2);

        const auto sum = std::accumulate(marker.begin(), marker.end(), cv::Point2f(0.f));
        const double ratio = image.cols / inputImage.cols;

        TrackedMarker info;
        info.id = marker.id;
        info.rawX = sum.x / marker.size() * ratio;
        info.rawY = sum.y / marker.size() * ratio;
        info.x = info.rawX / inputImage.cols - 0.5f;
        info.y = (1.f - info.rawY / inputImage.rows) - 0.5f;
        const auto side = marker[0] - marker[3];
        info.angle = std::atan2(side.x, side.y);
        info.size = len(side);

        result.push_back(info);
    }

    return result;
}
