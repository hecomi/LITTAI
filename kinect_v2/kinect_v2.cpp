#include "kinect_v2.h"

using namespace Littai;



KinectV2FrameWaitWorker::KinectV2FrameWaitWorker()
    : QObject()
    , isRunning_(false)
{
}


KinectV2FrameWaitWorker::~KinectV2FrameWaitWorker()
{
    stop();
}


void KinectV2FrameWaitWorker::setIrReader(IInfraredFrameReader *reader)
{
    irReader_ = reader;
}


void KinectV2FrameWaitWorker::start()
{
    isRunning_ = true;

    WAITABLE_HANDLE handle;
    auto hResult = irReader_->SubscribeFrameArrived(&handle);
    HANDLE events[] = { reinterpret_cast<HANDLE>(handle) };

    while (isRunning_) {
        WaitForMultipleObjects(1, events, true, INFINITE);
        newFrameArrived();
    }
}


void KinectV2FrameWaitWorker::stop()
{
    isRunning_ = false;
}



// ---


KinectV2::KinectV2(QQuickItem *parent)
    : Image(parent)
    , fps_(30)
    , isInitialized_(false)
{
    init();
    start();
}


KinectV2::~KinectV2()
{
    stop();
}


void KinectV2::init()
{
    IKinectSensor* kinect;
    if (errorCheck(::GetDefaultKinectSensor(&kinect))) return;
    kinect_ = std::shared_ptr<IKinectSensor>(kinect,
                [](IKinectSensor* ptr) { ptr->Close(); });
    if (errorCheck(kinect->Open())) return;

    IInfraredFrameSource* irSource;
    if (errorCheck(kinect_->get_InfraredFrameSource(&irSource))) return;
    irSource_ = std::shared_ptr<IInfraredFrameSource>(irSource,
                [](IInfraredFrameSource* ptr) { ptr->Release(); });

    IInfraredFrameReader* irReader;
    if (errorCheck(irSource_->OpenReader(&irReader))) return;
    irReader_ = std::shared_ptr<IInfraredFrameReader>(irReader,
                [](IInfraredFrameReader* ptr) { ptr->Release(); });

    IFrameDescription* irDesc;
    if (errorCheck(irSource_->get_FrameDescription(&irDesc))) return;
    if (errorCheck(irDesc->get_Width(&width_))) return;
    if (errorCheck(irDesc->get_Height(&height_))) return;

    data_ = std::vector<UINT16>(height_ * width_);
    isInitialized_ = true;
}


void KinectV2::start()
{
    worker_ = std::make_shared<KinectV2FrameWaitWorker>();
    worker_->setIrReader(irReader_.get());
    worker_->moveToThread(&workerThread_);
    connect(&workerThread_, &QThread::finished, worker_.get(), &QObject::deleteLater);
    connect(worker_.get(), &KinectV2FrameWaitWorker::newFrameArrived, [&] { fetch(); });
    connect(this, &KinectV2::waitStart, worker_.get(), &KinectV2FrameWaitWorker::start);
    connect(this, &KinectV2::waitStop, worker_.get(), &KinectV2FrameWaitWorker::stop);
    workerThread_.start();
    emit waitStart();
}


void KinectV2::stop()
{
    emit waitStop();
}


bool KinectV2::errorCheck(HRESULT result)
{
    if (result != S_OK) {
        qDebug() << "failed: " << std::hex << result;
        return true;
    }
    return false;
}


void KinectV2::fetch()
{
    if (!isInitialized_) return;

    IInfraredFrame* irFrame;
    if (FAILED(irReader_->AcquireLatestFrame(&irFrame))) {
        return;
    }

    std::vector<UINT16> data_(width_ * height_);
    if (errorCheck(irFrame->CopyFrameDataToArray(data_.size(), &data_[0]))) return;
    irFrame->Release();

    cv::Mat image(height_, width_, CV_16U, &data_[0]);
    cv::Mat outputImage;
    cv::convertScaleAbs(image, outputImage, 1.0 / 255);
    cv::cvtColor(outputImage, outputImage, cv::COLOR_GRAY2BGR);

    setImage(outputImage);
}
