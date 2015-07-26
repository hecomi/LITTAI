#include "kinect_v2.h"
#include <thread>

using namespace Littai;



KinectV2FrameReadWorker::KinectV2FrameReadWorker()
    : QObject()
    , width_(0)
    , height_(0)
    , isRunning_(false)
{
}


KinectV2FrameReadWorker::~KinectV2FrameReadWorker()
{
    stop();
}


void KinectV2FrameReadWorker::setIrReader(IInfraredFrameReader *reader)
{
    irReader_ = reader;
}


void KinectV2FrameReadWorker::setSize(int width, int height)
{
    width_  = width;
    height_ = height;
    data_   = std::vector<UINT16>(width * height);
}


const cv::Mat& KinectV2FrameReadWorker::getImage() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return image_;
}


void KinectV2FrameReadWorker::start()
{
    isRunning_ = true;

    WAITABLE_HANDLE handle;
    auto result = irReader_->SubscribeFrameArrived(&handle);
    if (result != S_OK) {
        qDebug() << "failed: " << std::hex << result;
        return;
    }

    // TODO: WaitForSingleObject でフレーム到着まで待つかと思ったけどそうではない...？
    using namespace std::chrono;
    auto t = high_resolution_clock::now();
    while (isRunning_) {
        const auto dt = duration_cast<nanoseconds>(high_resolution_clock::now() - t);
        auto waitTime = nanoseconds(static_cast<int>(1e9 / 30)) - dt;
        if (waitTime > nanoseconds::zero()) {
            std::this_thread::sleep_for(waitTime);
        }
        const auto ret = WaitForSingleObject(reinterpret_cast<HANDLE>(handle), 100);

        if (ret == WAIT_TIMEOUT) {
            qDebug() << "IR Frame Timeout";
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        } else if (ret != WAIT_OBJECT_0) {
            qDebug() << "IR Frame Error: " << GetLastError();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        IInfraredFrame* irFrame;
        if (FAILED(irReader_->AcquireLatestFrame(&irFrame))) {
            continue;
        }
        irFrame->CopyFrameDataToArray(data_.size(), &data_[0]);
        irFrame->Release();

        cv::Mat image(height_, width_, CV_16U, &data_[0]);
        cv::Mat outputImage;
        cv::convertScaleAbs(image, outputImage, 1.0 / 255);
        cv::cvtColor(outputImage, outputImage, cv::COLOR_GRAY2BGR);
        {
            std::lock_guard<std::mutex> lock(mutex_);
            outputImage.copyTo(image_);
        }

        emit newFrameArrived();
    }
}


void KinectV2FrameReadWorker::stop()
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
}


KinectV2::~KinectV2()
{
    stop();
}


void KinectV2::init()
{
    if (isInitialized_) return;

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

    start();

    isInitialized_ = true;
}


void KinectV2::start()
{
    worker_.setIrReader(irReader_.get());
    worker_.setSize(width_, height_);
    worker_.moveToThread(&workerThread_);
    connect(&worker_, SIGNAL(newFrameArrived()), this, SLOT(fetch()));
    connect(this, SIGNAL(waitStart()), &worker_, SLOT(start()));
    connect(this, SIGNAL(waitStop()),  &worker_, SLOT(stop()));
    workerThread_.start();
    emit waitStart();
}


void KinectV2::stop()
{
    emit waitStop();
    workerThread_.exit();
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

    setImage(worker_.getImage());
}
