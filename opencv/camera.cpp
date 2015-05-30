#include <chrono>
#include <mutex>
#include "camera.h"

using namespace Littai;



namespace {
    std::mutex mutex;
}



// ---



Camera::Fetcher::Fetcher(cv::VideoCapture &video)
    : video_(video)
{
}


Camera::Fetcher::~Fetcher()
{
}


void Camera::Fetcher::fetch()
{
    std::lock_guard<std::mutex> lock(mutex);
    if (video_.isOpened()) video_ >> image_;
}



// ---



Camera::SyncFetcher::SyncFetcher(cv::VideoCapture &video)
    : Fetcher(video)
{
}


cv::Mat Camera::SyncFetcher::get()
{
    fetch();
    return image_;
}



// ---


Camera::AsyncFetcher::AsyncFetcher(cv::VideoCapture &video)
    : Fetcher(video)
    , isRunning_(false)
    , fps_(30)
{
    using namespace std::chrono;

    thread_ = std::thread([&] {
        isRunning_ = true;
        while (isRunning_) {
            const auto t1 = high_resolution_clock::now();
            fetch();
            const auto t2 = high_resolution_clock::now();
            const auto dt = duration_cast<microseconds>(t2 - t1);

            auto waitTime = microseconds(1000000 / fps_) - dt;
            if (waitTime < microseconds::zero()) waitTime = microseconds::zero();

            std::this_thread::sleep_for(waitTime);
        }
    });
}


Camera::AsyncFetcher::~AsyncFetcher()
{
    if (thread_.joinable()) {
        isRunning_ = false;
        thread_.join();
    }
}


cv::Mat Camera::AsyncFetcher::get()
{
    return image_;
}



// ---



Camera::Camera(QQuickItem *parent)
    : Image(parent)
    , camera_(0)
    , fps_(15)
    , isAsync_(true)
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
    } else {
        setAsync(isAsync_);
    }
}


void Camera::close()
{
    std::lock_guard<std::mutex> lock(mutex);

    if ( video_.isOpened() ) {
        video_.release();
    }
}


bool Camera::isOpened() const
{
    return video_.isOpened();
}


void Camera::fetch()
{
    if ( isOpened() && fetcher_ ) {
        setImage(fetcher_->get());
    }
}


void Camera::paint(QPainter *painter)
{
    fetch();
    Image::paint(painter);
}


bool Camera::isAsync() const
{
    return isAsync_;
}


void Camera::setAsync(bool isAsync)
{
    if (isAsync_ == isAsync && fetcher_) return;
    isAsync_ = isAsync;

    if (isAsync) {
        fetcher_ = std::make_shared<AsyncFetcher>(video_);
    } else {
        fetcher_ = std::make_shared<SyncFetcher>(video_);
    }

    emit isAsyncChanged();
}
