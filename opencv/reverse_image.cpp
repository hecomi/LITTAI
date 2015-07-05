#include "reverse_image.h"

using namespace Littai;



ReverseImage::ReverseImage(QQuickItem *parent)
    : Image(parent)
{
}


void ReverseImage::setImage(const QVariant &image)
{
    auto inputImage = image.value<cv::Mat>().clone();
    if (horizontal_ && vertical_) {
        cv::flip(inputImage, inputImage, -1);
    } else if (horizontal_) {
        cv::flip(inputImage, inputImage, 0);
    } else if (vertical_) {
        cv::flip(inputImage, inputImage, 1);
    }
    Image::setImage(inputImage);
    emit imageChanged();
}
