#include "xtion.h"

using namespace Littai;



ImageListener::ImageListener(
    const std::shared_ptr<openni::Device>& device, openni::SensorType sensor)
    : isStarted_(false)
{
    stream_ = std::make_shared<openni::VideoStream>();
    stream_->create(*device, sensor);
    stream_->addNewFrameListener(this);
}


ImageListener::~ImageListener()
{
    if (stream_->isValid()) {
        std::lock_guard<std::mutex> lock(mutex_);
        stream_->stop();
        stream_->destroy();
    }
}


cv::Mat ImageListener::getImage() const
{
    return image_;
}


void ImageListener::setMode(int width, int height, int fps)
{
    if (!stream_->isValid()) return;

    auto mode = stream_->getVideoMode();
    mode.setResolution(width, height);
    mode.setFps(fps);
    stream_->setVideoMode(mode);
}


void ImageListener::start()
{
    stream_->start();
    isStarted_ = true;
}


void ImageListener::stop()
{
    stream_->start();
    isStarted_ = false;
}


bool ImageListener::isStarted() const
{
    return isStarted_;
}



// ---



IrImageListener::IrImageListener(
    const std::shared_ptr<openni::Device> &device)
    : ImageListener(device, openni::SENSOR_IR)
{
}


void IrImageListener::onNewFrame(openni::VideoStream &stream)
{
    openni::VideoFrameRef frame;
    stream.readFrame(&frame);

    if (!frame.isValid()) {
        qDebug() << "IR frame is invalid.";
        return;
    }

    cv::Mat image(
        frame.getHeight(),
        frame.getWidth(),
        CV_16U,
        static_cast<char*>( const_cast<void*>(frame.getData())) );
    image.convertTo(image, CV_8U);
    cv::flip(image, image, 1);

    {
        std::lock_guard<std::mutex> lock(mutex_);
        image_ = image;
    }
}


cv::Mat IrImageListener::getImage() const
{
    cv::Mat image;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        image_.copyTo(image);
    }
    cv::cvtColor(image, image, CV_GRAY2BGR);
    return image;
}



// ---



ColorImageListener::ColorImageListener(
    const std::shared_ptr<openni::Device> &device)
    : ImageListener(device, openni::SENSOR_COLOR)
{
}


void ColorImageListener::onNewFrame(openni::VideoStream &stream)
{
    std::lock_guard<std::mutex> lock(mutex_);

    openni::VideoFrameRef frame;
    stream.readFrame(&frame);

    if (!frame.isValid()) {
        qDebug() << "Color frame is invalid.";
        return;
    }

    cv::Mat image(
        frame.getHeight(),
        frame.getWidth(),
        CV_8UC3,
        static_cast<char*>( const_cast<void*>(frame.getData())) );
    cv::flip(image, image, 1);
    cv::cvtColor(image, image, CV_BGR2RGB);

    image_ = image;
}



cv::Mat ColorImageListener::getImage() const
{
    return image_;
}



// ---



std::shared_ptr<openni::Device> Xtion::device = nullptr;
int  Xtion::instanceNum    = 0;
bool Xtion::isDeviceOpened = false;


Xtion::Xtion(QQuickItem* parent)
    : Image(parent)
    , sensorType_(SensorType::None)
    , imageWidth_(640)
    , imageHeight_(480)
    , fps_(30)
{
    if (instanceNum == 0) initialize();
    ++instanceNum;
}


Xtion::~Xtion()
{
    --instanceNum;
    if (instanceNum == 0) shutdown();
}


void Xtion::start()
{
    if (!isOpened()) return;

    if (!listener_) {
        setSensorType(sensorType_);
    }
    if (listener_) {
        listener_->start();
    }
}


void Xtion::stop()
{
    if (!isOpened()) return;

    if (listener_) {
        listener_->stop();
    }
}


void Xtion::restart()
{
    listener_.reset();
    start();
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
        qDebug() << openni::OpenNI::getExtendedError();
        return;
    }

    isDeviceOpened = true;
}


void Xtion::shutdown()
{
    openni::OpenNI::shutdown();
    isDeviceOpened = false;
}


Xtion::SensorType Xtion::sensorType() const
{
    return sensorType_;
}


void Xtion::setSensorType(SensorType sensorType)
{
    if (!isOpened()) return;

    if (sensorType_ == sensorType && listener_) return;
    sensorType_ = sensorType;

    switch (sensorType) {
        case Xtion::SensorType::Ir:
            listener_ = std::make_shared<IrImageListener>(device);
            break;
        case Xtion::SensorType::Color:
            listener_ = std::make_shared<ColorImageListener>(device);
            break;
        case Xtion::SensorType::Depth:
            error("currently not supported...");
            break;
        default:
            error("invalid sensor.");
            break;
    }
    updateMode();

    emit sensorTypeChanged();
}


int Xtion::imageWidth() const
{
    return imageWidth_;
}


void Xtion::setImageWidth(int width)
{
    if (imageWidth_ == width) return;
    imageWidth_ = width;
    updateMode();
    emit imageWidthChanged();
}


int Xtion::imageHeight() const
{
    return imageHeight_;
}


void Xtion::setImageHeight(int height)
{
    if (imageHeight_ == height) return;
    imageHeight_ = height;
    updateMode();
    emit imageHeightChanged();
}


int Xtion::fps() const
{
    return fps_;
}


void Xtion::setFps(int fps)
{
    if (fps_ == fps) return;
    fps_ = fps;
    updateMode();
    emit fpsChanged();
}


void Xtion::updateMode()
{
    if (!listener_) return;
    listener_->setMode(imageWidth_, imageHeight_, fps_);
}


void Xtion::fetch()
{
    if (isOpened() && listener_) {
        auto image = listener_->getImage();
        setImage(image);
    }
}


void Xtion::paint(QPainter *painter){
    Image::paint(painter);
}
