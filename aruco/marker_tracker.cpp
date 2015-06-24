#include <numeric>
#include <aruco.h>
#include <opencv2/opencv.hpp>
#include "marker_tracker.h"

using namespace Littai;



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
                marker.x = newMarker.x;
                marker.y = newMarker.y;
                marker.angle = newMarker.angle;
                marker.checked = true;
                isFound = true;
                break;
            }
        }
        if (!isFound) {
            qDebug() << newMarker.id;
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

    qDebug() << markers_.size();
    for (auto&& marker : markers_) {
        marker.print();
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
    cv::resize(inputImage, inputImage, cv::Size(), 1.5, 1.5, cv::INTER_LINEAR);

    // カメラパラメタのロード
    aruco::CameraParameters params;
    params.readFromXMLFile(cameraParamsFilePath_.toStdString());
    params.resize(inputImage.size());

    // マーカを認識する
    aruco::MarkerDetector detector;
    std::vector<aruco::Marker> markers;
    const float markerSize = 0.03f;
    detector.detect(inputImage, markers, params, markerSize);

    // 結果を書き出す
    std::vector<TrackedMarker> result;
    for (auto&& marker : markers) {
        marker.draw(inputImage, cv::Scalar(0, 0, 255), 2);
        const auto sum = std::accumulate(
            marker.begin(),
            marker.end(),
            cv::Point2f(0.f));

        TrackedMarker info;
        info.id = marker.id;
        info.x = sum.x / marker.size() / inputImage.cols - 0.5f;
        info.y = (1.f - sum.y / marker.size() / inputImage.rows) - 0.5f;
        const auto side = marker[0] - marker[3];
        info.angle = std::atan2(side.x, side.y);

        result.push_back(info);
    }

    return result;
}
