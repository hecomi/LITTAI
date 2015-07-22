#include "image.h"

using namespace Littai;



Image::Image(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
}


QVariant Image::image() const
{
    std::lock_guard<std::mutex> lock(imageMutex_);
    return QVariant::fromValue(image_);
}


void Image::setImage(const QVariant &image)
{
    setImage( image.value<cv::Mat>() );
}


void Image::setImage(const cv::Mat &mat, bool isUpdate)
{
    if ( mat.empty() ) {
        error("image is empty.");
        return;
    }
    {
        std::lock_guard<std::mutex> lock(imageMutex_);
        mat.copyTo(image_);
    }

    emit imageChanged();
    emit imageWidthChanged();
    emit imageHeightChanged();
    if (isUpdate) {
        emit update();
    }
}


int Image::imageWidth() const
{
    std::lock_guard<std::mutex> lock(imageMutex_);
    if (image_.empty()) return -1;
    return image_.cols;
}


int Image::imageHeight() const
{
    std::lock_guard<std::mutex> lock(imageMutex_);
    if (image_.empty()) return -1;
    return image_.rows;
}


QString Image::filePath() const
{
    return filePath_;
}


void Image::setFilePath(const QString& path)
{
    auto img = cv::imread( path.toStdString() );
    if (img.empty()) {
        qDebug() << (path + " is not found");
        error(path + " is not found");
        return;
    }

    filePath_ = path;
    emit filePathChanged();

    setImage(img);
}


void Image::paint(QPainter *painter)
{
    {
        std::lock_guard<std::mutex> lock(imageMutex_);
        if ( image_.empty() || !isVisible() ) return;
    }

    int w = width();
    int h = height();

    cv::Mat image;
    {
        std::lock_guard<std::mutex> lock(imageMutex_);
        image = image_.clone();
    }
    cv::Mat scaledImage;
    cv::resize(image, scaledImage, cv::Size(w, h), CV_INTER_NN);
    cv::cvtColor(scaledImage, scaledImage, cv::COLOR_BGR2RGB);

    const QImage outputImage(scaledImage.data, w, h, 3 * w, QImage::Format_RGB888);
    painter->drawImage(0, 0, outputImage);
}
