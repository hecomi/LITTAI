#include "diff_image.h"

using namespace Littai;



DiffImage::DiffImage(QQuickItem *parent)
    : Image(parent)
{
}


void DiffImage::setBaseImage(const QVariant &image)
{
    auto baseImage = image.value<cv::Mat>();
    if (baseImage.empty()) return;
    baseImage_ = baseImage;

    cv::Mat gray;
    cv::cvtColor(baseImage_, gray, cv::COLOR_BGR2GRAY);
    float kernelData[] = {
        -1/5.f, -1/5.f, -1/5.f,
        -1/5.f, 13/5.f, -1/5.f,
        -1/5.f, -1/5.f, -1/5.f,
    };
    cv::Mat filter(cv::Size(3, 3), CV_32F, kernelData);
    cv::filter2D(gray, gray, gray.depth(), filter);
    cv::cvtColor(gray, baseImage_, cv::COLOR_GRAY2BGR);

    emit baseImageChanged();
}


QVariant DiffImage::baseImage() const
{
    return QVariant::fromValue(baseImage_);
}


void DiffImage::setInputImage(const QVariant &image)
{
    image.value<cv::Mat>().copyTo(inputImage_);
    emit inputImageChanged();

    if (baseImage_.empty()) {
        setImage(inputImage_);
    } else {
        cv::Mat image;

        cv::Mat gray;
        cv::cvtColor(inputImage_, gray, cv::COLOR_BGR2GRAY);
        float kernelData[] = {
            -1/5.f, -1/5.f, -1/5.f,
            -1/5.f, 13/5.f, -1/5.f,
            -1/5.f, -1/5.f, -1/5.f,
        };
        cv::Mat filter(cv::Size(3, 3), CV_32F, kernelData);
        cv::filter2D(gray, gray, gray.depth(), filter);
        cv::cvtColor(gray, inputImage_, cv::COLOR_GRAY2BGR);

        cv::subtract(inputImage_, baseImage_, image);
        setImage(image);
    }
}


QVariant DiffImage::inputImage() const
{
    return QVariant::fromValue(inputImage_);
}
