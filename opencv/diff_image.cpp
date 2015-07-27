#include "diff_image.h"

using namespace Littai;



DiffImage::DiffImage(QQuickItem *parent)
    : Image(parent)
    , gamma_(1.0)
    , sharpness_(1.f)
    , intensityCorrectionMin_(50.0)
    , intensityCorrectionMax_(250.0)
{
}


void DiffImage::unsharpMask(cv::Mat &image, float k)
{
    float kernelData[] = {
        -k/9.0f, -k/9.0f,           -k/9.0f,
        -k/9.0f, 1 + (8 * k)/9.0f,  -k/9.0f,
        -k/9.0f, -k/9.0f,           -k/9.0f,
    };
    cv::Mat filter(cv::Size(3, 3), CV_32F, kernelData);
    cv::filter2D(image, image, image.depth(), filter);
}


void DiffImage::setBaseImage(const QVariant &image)
{
    auto baseImage = image.value<cv::Mat>();
    if (baseImage.empty()) return;

    unsharpMask(baseImage, sharpness_);

    cv::Mat gray;
    cv::cvtColor(baseImage, gray, cv::COLOR_BGR2GRAY);

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
        unsharpMask(gray, sharpness_);
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
    // 100 を基準にどれだけ得られた画像の強度を変化させるか（200 なら 2 倍）
    cv::Mat image(baseImage_.size(), baseImage_.type());
    for (int x = 0; x < image.cols; ++x) {
        for (int y = 0; y < image.rows; ++y) {
            for (int c = 0; c < image.channels(); ++c) {
                const int index = y * image.step + x * image.elemSize() + c;
                image.data[index] = intensityCorrectionMin_ + (intensityCorrectionMax_ - intensityCorrectionMin_) * (1.0 - 1.0 * y / image.rows);
            }
        }
    }
    intensityCorrectionImage_ = image.clone();
    /*
    auto image = baseImage_.clone();
    const int cols = image.cols;
    const int rows = image.rows;
    const int blur = 31;
    const int margin = 30;
    cv::medianBlur(image, image, blur);
    image = image(cv::Rect(cv::Point(margin, margin), cv::Point(rows - margin, cols - margin))).clone();
    cv::resize(image, intensityCorrectionImage_, cv::Size(rows, cols));
    */
    emit intensityCorrectionImageChanged();
}


void DiffImage::applyIntensityCorrection(cv::Mat &image)
{
    if (image.total() != intensityCorrectionImage_.total()) return;

    for (int x = 0; x < image.cols; ++x) {
        for (int y = 0; y < image.rows; ++y) {
            for (int c = 0; c < image.channels(); ++c) {
                const int index = y * image.step + x * image.elemSize() + c;
                auto val = image.data[index] * (1.0 * intensityCorrectionImage_.data[index] / 100);
                if (val > 255) val = 255;
                image.data[index] = static_cast<int>(pow(val / 255.0, gamma_) * 255);
            }
        }
    }
}
