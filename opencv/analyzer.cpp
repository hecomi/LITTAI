#include "analyzer.h"

using namespace Littai;



Analyzer::Analyzer(QQuickItem *parent)
    : Image(parent)
    , threshold_(128)
{
}


void Analyzer::setBaseImage(const QVariant &image)
{
    image.value<cv::Mat>().copyTo(baseImage_);
    cv::medianBlur(baseImage_, baseImage_, 7);
    emit baseImageChanged();
}


QVariant Analyzer::baseImage() const
{
    return QVariant::fromValue(baseImage_);
}


void Analyzer::setInputImage(const QVariant &image)
{
    image.value<cv::Mat>().copyTo(inputImage_);
    emit inputImageChanged();

    if (baseImage_.empty()) {
        setImage(inputImage_);
    } else {
        cv::Mat image;
        cv::absdiff(inputImage_, baseImage_, image);

        cv::Mat lut(1, 256, CV_8U);
        for (int i = 0; i < 256; ++i) {
            const auto val = std::pow(i, 2);
            lut.at<unsigned char>(i) = (i < threshold_) ?
                (val / threshold_) :
                (256 - val / std::pow(256, 2));
        }
        cv::LUT(image, lut, image);
        cv::medianBlur(image, image, 7);
        cv::threshold(image, image, threshold_, 255, cv::THRESH_BINARY);

        setImage(image);
    }
}


QVariant Analyzer::inputImage() const
{
    return QVariant::fromValue(inputImage_);
}
