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
    detectUsingAruco(inputImage);

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


void MarkerTracker::detectUsingAruco(cv::Mat &inputImage)
{
    cv::resize(inputImage, inputImage, cv::Size(), 1.5, 1.5, cv::INTER_NEAREST);

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
    for (auto&& marker : markers) {
        marker.draw(inputImage, cv::Scalar(0, 0, 255), 2);
    }
}
