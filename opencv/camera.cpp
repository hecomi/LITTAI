#include "camera.h"

using namespace Littai;



Camera::Camera(QQuickItem *parent)
    : Image(parent)
    , camera_(0)
    , fps_(30)
{
}


Camera::~Camera()
{
    close();
}


void Camera::open()
{
    close();
    if ( !video_.open(camera_) ) {
        const auto msg = QString("try to open camera ") + camera_ + ", but failed...";
        error(msg);
    }
}


void Camera::close()
{
    if ( video_.isOpened() ) {
        video_.release();
    }
}


bool Camera::isOpened() const
{
    return video_.isOpened();
}


void Camera::paint(QPainter *painter){
    if ( isOpened() ) {
        cv::Mat img;
        video_ >> img;
        setImage(img);
    }
    Image::paint(painter);
}
