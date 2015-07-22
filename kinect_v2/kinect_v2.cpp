#include "kinect_v2.h"

using namespace Littai;



KinectV2::KinectV2(QQuickItem *parent)
    : Image(parent)
    , fps_(30)
    , isInitialized_(false)
{
    init();
}


KinectV2::~KinectV2()
{
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
