#include "xtion.h"

using namespace Littai;



const cv::Mat& ImageListener::getImage() const
{
    return image_;
}



// ---



void IRImageListener::onNewFrame(openni::VideoStream &stream)
{
    openni::VideoFrameRef frame;
    stream.readFrame(&frame);

    if (!frame.isValid()) {
        qDebug() << "IR frame is invalid.";
        return;
    }

    cv::Mat image(frame.getHeight(), frame.getWidth(),
                  CV_16U, static_cast<char*>( const_cast<void*>(frame.getData())) );
    image.convertTo(image, CV_8U);
    cv::flip(image, image, 1);

    image_ = image;
}



// ---



void ColorImageListener::onNewFrame(openni::VideoStream &stream)
{
    openni::VideoFrameRef frame;
    stream.readFrame(&frame);

    if (!frame.isValid()) {
        qDebug() << "Color frame is invalid.";
        return;
    }

    cv::Mat image(frame.getHeight(), frame.getWidth(),
                  CV_8UC3, static_cast<char*>( const_cast<void*>(frame.getData())) );
    cv::flip(image, image, 1);

    image_ = image;
}



// ---



std::shared_ptr<openni::Device>      Xtion::device        = nullptr;
std::shared_ptr<IRImageListener>     Xtion::irListener    = nullptr;
std::shared_ptr<openni::VideoStream> Xtion::irStream      = nullptr;
std::shared_ptr<ColorImageListener>  Xtion::colorListener = nullptr;
std::shared_ptr<openni::VideoStream> Xtion::colorStream   = nullptr;

int  Xtion::instanceNum    = 0;
bool Xtion::isDeviceOpened = false;


Xtion::Xtion(QQuickItem* parent)
    : Image(parent), isColor_(false)
{
    if (instanceNum == 0) {
        initialize();
    }
    ++instanceNum;
}


Xtion::~Xtion()
{
    --instanceNum;
    if (instanceNum == 0) {
        shutdown();
    }
}


bool Xtion::isOpened() const
{
    return isDeviceOpened;
}


void Xtion::initialize()
{
    openni::OpenNI::initialize();

    device = std::make_shared<openni::Device>();
    if (device->open(openni::ANY_DEVICE) != openni::STATUS_OK) {
        emit error(openni::OpenNI::getExtendedError());
        return;
    }

    irListener = std::make_shared<IRImageListener>();
    irStream = std::make_shared<openni::VideoStream>();
    irStream->create(*device, openni::SENSOR_IR);
    irStream->addNewFrameListener(irListener.get());
    auto irMode = irStream->getVideoMode();
    irMode.setResolution(640, 480);
    irMode.setFps(30);
    irStream->setVideoMode(irMode);
    irStream->start();

    // Color Streams DOES NOT work with IR cam. ...x(
    colorListener = std::make_shared<ColorImageListener>();
    colorStream = std::make_shared<openni::VideoStream>();
    colorStream->create(*device, openni::SENSOR_COLOR);
    colorStream->addNewFrameListener(colorListener.get());
    auto colorMode = irStream->getVideoMode();
    colorMode.setResolution(640, 480);
    colorMode.setFps(30);
    colorStream->setVideoMode(colorMode);
    colorStream->start();

    isDeviceOpened = true;
    emit open();
}


void Xtion::shutdown()
{
    openni::OpenNI::shutdown();
    isDeviceOpened = false;
    emit close();
}


void Xtion::paint(QPainter *painter){
    if ( isOpened() ) {
        if (!isColor_) {
            auto image = irListener->getImage();
            cv::cvtColor(image, image_, CV_GRAY2BGR);
            emit imageChanged();
        } else { // color stream does not work...
            image_ = colorListener->getImage();
            emit imageChanged();
        }
    }
    Image::paint(painter);
}
