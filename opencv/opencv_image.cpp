#include "opencv_image.h"

using namespace Littai;


OpenCVImage::OpenCVImage(QQuickItem *parent) :
    QQuickPaintedItem(parent)
{
}


QVariant OpenCVImage::image() const
{
    return QVariant::fromValue(image_);
}


void OpenCVImage::setImage(const QVariant &image)
{
    auto mat = image.value<cv::Mat>();
    if ( mat.empty() ) {
        error("image is empty.");
        return;
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        image_ = mat;
    }

    emit imageChanged();
    emit update();
}


int OpenCVImage::imageWidth() const
{
    if (image_.empty()) return -1;
    return image_.size().width;
}


int OpenCVImage::imageHeight() const
{
    if (image_.empty()) return -1;
    return image_.size().height;
}


QString OpenCVImage::filePath() const
{
    return filePath_;
}


void OpenCVImage::setFilePath(const QString& path)
{
    auto img = cv::imread( path.toStdString() );
    if (img.empty()) {
        error(path + " is not found");
        return;
    }

    filePath_ = path;
    emit filePathChanged();

    setImage(QVariant::fromValue(img));
}


void OpenCVImage::paint(QPainter *painter)
{

    if ( image_.empty() ) return;

    cv::Mat scaledImage(height(), width(), image_.type());
    {
        std::lock_guard<std::mutex> lock(mutex_);
        cv::resize(image_, scaledImage, scaledImage.size(), cv::INTER_CUBIC);
    }

    // BGR -> ARGB
    cv::cvtColor(scaledImage, scaledImage, CV_BGR2BGRA);
    std::vector<cv::Mat> bgra;
    cv::split(scaledImage, bgra);
    std::swap(bgra[0], bgra[3]);
    std::swap(bgra[1], bgra[2]);

    QImage outputImage(scaledImage.data, scaledImage.cols, scaledImage.rows, QImage::Format_ARGB32);
    painter->drawImage(0, 0, outputImage);
}
