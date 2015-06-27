#include "homography.h"

using namespace Littai;



Homography::Homography(QQuickItem *parent)
    : Image(parent)
    , width_(-1)
    , height_(-1)
{
}


void Homography::setImage(const QVariant& image)
{
    if (srcPoints_.empty()) {
        Image::setImage(image);
        return;
    }

    const cv::Mat srcImage = image.value<cv::Mat>();
    const int width  = (width_  <= 0) ? srcImage.rows : width_;
    const int height = (height_ <= 0) ? srcImage.cols : height_;
    cv::Mat destImage(width, height, srcImage.type());

    std::vector<double> srcPointVec;
    for (const QVariant& data : srcPoints_) {
        const auto p = data.value<QVariantList>();
        const auto x = p[0].value<double>();
        const auto y = p[1].value<double>();
        srcPointVec.push_back(x * srcImage.cols);
        srcPointVec.push_back(y * srcImage.rows);
    }

    double destPointArray[] = {
        0.0,                  0.0,
        destImage.cols - 1.0, 0.0,
        destImage.cols - 1.0, destImage.rows - 1.0,
        0.0,                  destImage.rows - 1.0
    };

    const cv::Mat srcPoints(srcPoints_.length(),  2, CV_64FC1, &srcPointVec[0]);
    const cv::Mat destPoints(srcPoints_.length(), 2, CV_64FC1, destPointArray);

    auto homographyMat = cv::findHomography(srcPoints, destPoints);
    cv::warpPerspective(srcImage, destImage, homographyMat, destImage.size());

    Image::setImage(destImage);

    emit imageChanged();
}
