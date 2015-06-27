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

    cv::Mat image, gray;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        image = inputImage_.clone();
    }

    preProcess(image);
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    detectMarkers(image, gray);
    detectPolygons(image, gray);

    setImage(image);
}


void MarkerTracker::preProcess(cv::Mat &image)
{
    cv::Mat lut(1, 256, CV_8U);
    for (int i = 0; i < 256; ++i) {
        const auto val = std::pow(i, 2);
        lut.at<unsigned char>(i) = (i < contrastThreshold_) ?
            val / contrastThreshold_ :
            (256 - val / std::pow(256, 2));
    }
    cv::LUT(image, lut, image);

    imageCaches_.push_back(image);
    if (imageCaches_.size() > 1) {
        imageCaches_.pop_front();
    }
    cv::Mat input(image.rows, image.cols, image.type(), cv::Scalar(0));
    for (auto&& cache : imageCaches_) {
        input += cache * 1.0 / imageCaches_.size();
    }
    image = input;
}


void MarkerTracker::detectMarkers(cv::Mat &resultImage, cv::Mat &inputImage)
{
    // 認識できるように拡大
    const double scale = 1.5;
    cv::Mat image;
    cv::resize(inputImage, image, cv::Size(), scale, scale, cv::INTER_LINEAR);

    // AruCo によるマーカの認識
    aruco::MarkerDetector detector;
    std::vector<aruco::Marker> markers;
    detector.detect(image, markers);

    // 結果を格納
    std::vector<TrackedMarker> newMarkers;
    for (auto&& marker : markers) {
        const auto sum = std::accumulate(marker.begin(), marker.end(), cv::Point2f(0.f));

        TrackedMarker info;
        info.id = marker.id;
        info.rawX = sum.x / marker.size() / scale;
        info.rawY = sum.y / marker.size() / scale;
        info.x = info.rawX / inputImage.cols - 0.5f;
        info.y = (1.f - info.rawY / inputImage.rows) - 0.5f;
        const auto side = marker[0] - marker[3];
        info.angle = std::atan2(side.x, side.y);
        info.size = len(side);

        newMarkers.push_back(info);
    }

    // フラグをオフにしておく
    for (auto&& marker : markers_) {
        marker.checked = false;
    }

    // 見つかったアイテムは情報を更新してフラグを立てる
    for (auto&& newMarker : newMarkers) {
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
}


void MarkerTracker::detectPolygons(cv::Mat &resultImage, cv::Mat &inputImage)
{
    cv::Mat image;
    cv::medianBlur(inputImage, image, 3);
    cv::threshold(image, image, 10, 255, cv::THRESH_OTSU);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(image, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_TC89_KCOS);

    // マーカを囲む形状を認識
    std::map<unsigned int, std::vector<cv::Point>> contourMap;
    if (contours.size() > 0 && markers_.size() > 0) {
        // 領域を大きい順に並べる
        std::sort(contours.begin(), contours.end(),
            [](const std::vector<cv::Point>& a, const std::vector<cv::Point>& b) {
                return cv::contourArea(a) > cv::contourArea(b);
            });

        // マーカを内包する領域を調べる
        for (auto&& marker : markers_) {
            bool isFound = false;
            for (const auto& contour : contours) {
                const cv::Point pt(marker.rawX, marker.rawY);
                if (cv::pointPolygonTest(contour, pt, 0) == 1) {
                    contourMap.emplace(marker.id, contour);
                    isFound = true;
                    break;
                }
            }
            if (!isFound) {
                qDebug("marker detected but contour not detected.");
            }
        }
    }

    for (auto&& marker : markers_) {
        // 中心と領域を描画
        const cv::Point center(marker.rawX, marker.rawY);
        cv::circle(resultImage, center, marker.size / 2, cv::Scalar(0, 255, 0), 2);

        auto it = contourMap.find(marker.id);
        if (it == contourMap.end()) return;
        auto& contour = (*it).second;

        std::vector<std::vector<cv::Point>> contours = { contour };
        cv::drawContours(resultImage, contours, 0, cv::Scalar(255, 0, 0), 3);

        std::vector<cv::Point> polygon;
        cv::approxPolyDP(contour, polygon, 5, true);
        marker.polygon = polygon;

        for (const auto& vertex : polygon) {
            const auto d = center - vertex;
            if (len(d) > 60) {
                cv::circle(resultImage, vertex, 4, cv::Scalar(0, 0, 255), -1);
            } else {
                cv::circle(resultImage, vertex, 2, cv::Scalar(255, 0, 255), -1);
            }
        }

        // 突端認識
        if (polygon.size() >= 4) {
            std::vector<cv::Point> edges;
            const int N = polygon.size();
            for (int i = 0; i < N; ++i) {
                const int i0 = i;
                const int i1 = ((i + 1) < N) ? (i + 1) : (i + 1 - N);
                const int i2 = ((i + 2) < N) ? (i + 2) : (i + 2 - N);
                const int i3 = ((i + 3) < N) ? (i + 3) : (i + 3 - N);

                const auto v0 = polygon[i0];
                const auto v1 = polygon[i1];
                const auto v2 = polygon[i2];
                const auto v3 = polygon[i3];

                const auto s1 = v1 - v0;
                const auto s2 = v2 - v1;
                const auto s3 = v3 - v2;

                const auto center = (v0 + v1 + v2 + v3) * 0.25;
                const double ratio = 0.7;

                const bool isMiddleShort = len(s2) / len(s1) < ratio && len(s2) / len(s3) < ratio;
                const bool isOpposite    = s1.dot(s3) < -0.5;
                const bool isMidInner    = image.at<unsigned char>(center.y, center.x) > 0;

                if (isMiddleShort && isOpposite && isMidInner) {
                    edges.push_back( (v1 + v2) * 0.5 );
                }
            }
            marker.edges = edges;

            for (const auto& edge : edges) {
                cv::circle(resultImage, edge, 6, cv::Scalar(0, 255, 0), -1);
            }
        }
    }
}
