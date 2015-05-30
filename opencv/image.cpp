#include "image.h"

using namespace Littai;



Image::Image(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
}


QVariant Image::image() const
{
    return QVariant::fromValue(image_);
}


void Image::setImage(const QVariant &image)
{
    setImage( image.value<cv::Mat>() );
}


void Image::setImage(const cv::Mat &mat)
{
    if ( mat.empty() ) {
        error("image is empty.");
        return;
    }
    image_ = mat;

    emit imageChanged();
    emit imageWidthChanged();
    emit imageHeightChanged();
    emit update();
}


int Image::imageWidth() const
{
    if (image_.empty()) return -1;
    return image_.size().width;
}


int Image::imageHeight() const
{
    if (image_.empty()) return -1;
    return image_.size().height;
}


QString Image::filePath() const
{
    return filePath_;
}


void Image::setFilePath(const QString& path)
{
    auto img = cv::imread( path.toStdString() );
    if (img.empty()) {
        error(path + " is not found");
        return;
    }

    filePath_ = path;
    emit filePathChanged();

    setImage(img);
}


void Image::paint(QPainter *painter)
{
    if ( image_.empty() ) return;

    cv::Mat scaledImage(height(), width(), image_.type());
    cv::resize(image_, scaledImage, scaledImage.size(), cv::INTER_CUBIC);

    QImage outputImage(scaledImage.data, scaledImage.cols, scaledImage.rows, QImage::Format_ARGB32);
    painter->drawImage(0, 0, outputImage);
}
