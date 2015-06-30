#include "diff_image.h"

using namespace Littai;



DiffImage::DiffImage(QQuickItem *parent)
    : Image(parent)
    , gamma_(1.0)
    , intensityPower_(1.0)
{
}


void DiffImage::setBaseImage(const QVariant &image)
{
    auto baseImage = image.value<cv::Mat>();
    if (baseImage.empty()) return;

    cv::Mat gray;
    cv::cvtColor(baseImage, gray, cv::COLOR_BGR2GRAY);

    float kernelData[] = {
        -1/5.f, -1/5.f, -1/5.f,
        -1/5.f, 13/5.f, -1/5.f,
        -1/5.f, -1/5.f, -1/5.f,
    };
    cv::Mat filter(cv::Size(3, 3), CV_32F, kernelData);
    cv::filter2D(gray, gray, gray.depth(), filter);

    baseImage_ = gray;

    emit baseImageChanged();

    createIntensityCorrectionImage();
}


QVariant DiffImage::baseImage() const
{
    cv::Mat color;
    cv::cvtColor(baseImage_, color, cv::COLOR_GRAY2BGR);
    return QVariant::fromValue(color);
}


QVariant DiffImage::intensityCorrectionImage() const
{
    cv::Mat color;
    cv::cvtColor(intensityCorrectionImage_, color, cv::COLOR_GRAY2BGR);
    return QVariant::fromValue(color);
}


void DiffImage::setInputImage(const QVariant &image)
{
    image.value<cv::Mat>().copyTo(inputImage_);
    emit inputImageChanged();

    if (baseImage_.empty()) {
        setImage(inputImage_);
    } else {
        cv::Mat gray;
        cv::cvtColor(inputImage_, gray, cv::COLOR_BGR2GRAY);
        float kernelData[] = {
            -1/5.f, -1/5.f, -1/5.f,
            -1/5.f, 13/5.f, -1/5.f,
            -1/5.f, -1/5.f, -1/5.f,
        };
        cv::Mat filter(cv::Size(3, 3), CV_32F, kernelData);
        cv::filter2D(gray, gray, gray.depth(), filter);
        cv::subtract(gray, baseImage_, gray);
        applyIntensityCorrection(gray);

        cv::Mat outputImage;
        cv::cvtColor(gray, outputImage, cv::COLOR_GRAY2BGR);

        setImage(outputImage);
    }
}


QVariant DiffImage::inputImage() const
{
    return QVariant::fromValue(inputImage_);
}


void DiffImage::createIntensityCorrectionImage()
{
    cv::Mat image;
    image = baseImage_.clone();
    const int cols = image.cols;
    const int rows = image.rows;
    const int blur = 31;
    const int margin = 30;
    cv::medianBlur(image, image, blur);
    auto inner = image(
        cv::Rect(cv::Point(margin, margin), cv::Point(rows - margin, cols - margin)));
    cv::resize(inner, intensityCorrectionImage_, cv::Size(rows, cols));
    emit intensityCorrectionImageChanged();
}


void DiffImage::applyIntensityCorrection(cv::Mat &image)
{
    if (image.total() != intensityCorrectionImage_.total()) return;

    /*
    cv::Mat lut(1, 256, CV_8U);
    for (int i = 0; i < 256; ++i) {
        lut.at<unsigned char>(i) = pow(1.0 * i / 255, gamma_) * 255;
    }
    cv::LUT(image, lut, image);
    */

    for (int x = 0; x < image.cols; ++x) {
        for (int y = 0; y < image.rows; ++y) {
            for (int c = 0; c < image.channels(); ++c) {
                const int index = y * image.step + x * image.elemSize() + c;
                const int intensity = std::pow(intensityCorrectionImage_.data[index], intensityPower_);
                const int val = image.data[index];
                image.data[index] = pow(1.0 * val / 255, gamma_ * intensity / 255) * 255;
            }
        }
    }
}
