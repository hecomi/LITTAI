#include "realsense.h"

using namespace Littai;



RealSense::RealSense(QQuickItem *parent)
    : Image(parent)
    , fps_(30)
    , isRunning_(false)
{
    init();
}


RealSense::~RealSense()
{
    stop();
}


void RealSense::start()
{
    if (isRunning_) return;
    isRunning_ = true;

    thread_ = std::thread([&] {
        using namespace std::chrono;
        while (isRunning_) {
            const auto t1 = high_resolution_clock::now();
            asyncUpdate();
            const auto t2 = high_resolution_clock::now();
            const auto dt = duration_cast<microseconds>(t2 - t1);

            auto waitTime = microseconds(1000000 / fps_) - dt;
            if (waitTime > microseconds::zero()) {
                std::this_thread::sleep_for(waitTime);
            }
        }
    });

}


void RealSense::stop()
{
    if (thread_.joinable()) {
        isRunning_ = false;
        thread_.join();
    }
}


void RealSense::init()
{
    senseManager_ = std::shared_ptr<PXCSenseManager>(
        PXCSenseManager::CreateInstance(), [](PXCSenseManager* ptr) { ptr->Close(); });
    if (!senseManager_) {
        error("failed to initialize SenseManager.");
        return;
    }

    {
        const auto status = senseManager_->EnableStream(
            PXCCapture::StreamType::STREAM_TYPE_IR, 640, 480, fps_);
        if (status < PXC_STATUS_NO_ERROR) {
            error("failed to enable IR stream.");
            return;
        }
    }

    {
        const auto status = senseManager_->Init();
        if (status < PXC_STATUS_NO_ERROR) {
            error("failed to init pipeline.");
            return;
        }
    }
}


void RealSense::error(const QString& msg)
{
    const auto err = "RealSense: " + msg;
    emit Image::error(err);
    qDebug() << err;
}


void RealSense::asyncUpdate()
{
    if (!senseManager_->IsConnected()) return;

    auto acquired = senseManager_->AcquireFrame(true);
    if (acquired < PXC_STATUS_NO_ERROR) return;

    const auto sample = senseManager_->QuerySample();
    if (sample == nullptr) return;

    const auto ir = sample->ir;
    PXCImage::ImageData data;
    const auto accessed = ir->AcquireAccess(
        PXCImage::Access::ACCESS_READ,
        PXCImage::PixelFormat::PIXEL_FORMAT_Y8,
        &data);
    if (accessed < PXC_STATUS_NO_ERROR) {
        error("failed to retrieve IR image.");
        return;
    }

    const auto info = ir->QueryInfo();
    cv::Mat image(info.height, info.width, CV_8U);
    memcpy(image.data, data.planes[0], info.height * info.width);
    {
        //std::lock_guard<std::mutex> lock(mutex_);
        if (!image.empty()) {
            image.copyTo(irImage_);
        }
    }

    ir->ReleaseAccess(&data);
    senseManager_->ReleaseFrame();
}


void RealSense::fetch()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (irImage_.empty()) return;

    cv::Mat image;
    cv::cvtColor(irImage_, image, cv::COLOR_GRAY2BGR);
    setImage(image);
}
