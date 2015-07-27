#include "reverse_image.h"

using namespace Littai;



ReverseImage::ReverseImage(QQuickItem *parent)
    : Image(parent)
{
}


QVariant ReverseImage::inputImage() const
{
    std::lock_guard<std::mutex> lock_guard(mutex_);
    return QVariant::fromValue(inputImage_);
}


void ReverseImage::setInputImage(const QVariant &image)
{
    {
        std::lock_guard<std::mutex> lock_guard(mutex_);
        inputImage_ = image.value<cv::Mat>().clone();
    }
    emit inputImageChanged();

    cv::Mat reversedImage;
    if (horizontal_ && vertical_) {
        cv::flip(inputImage_, reversedImage, -1);
    } else if (horizontal_) {
        cv::flip(inputImage_, reversedImage, 0);
    } else if (vertical_) {
        cv::flip(inputImage_, reversedImage, 1);
    } else {
        inputImage_.copyTo(reversedImage);
    }

    setImage(reversedImage);
}
