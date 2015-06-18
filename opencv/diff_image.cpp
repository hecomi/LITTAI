#include "diff_image.h"

using namespace Littai;



DiffImage::DiffImage(QQuickItem *parent)
    : Image(parent)
{
}


void DiffImage::setBaseImage(const QVariant &image)
{
    image.value<cv::Mat>().copyTo(baseImage_);
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
        cv::subtract(inputImage_, baseImage_, image);
        setImage(image);
    }
}


QVariant DiffImage::inputImage() const
{
    return QVariant::fromValue(inputImage_);
}
