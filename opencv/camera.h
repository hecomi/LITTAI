#ifndef CAMERA_H
#define CAMERA_H

#include "image.h"
#include <thread>
#include <memory>


namespace Littai
{


class Camera : public Image
{
    Q_OBJECT
    Q_PROPERTY(int fps MEMBER fps_ NOTIFY fpsChanged)
    Q_PROPERTY(int camera MEMBER camera_ NOTIFY deviceChanged)
    Q_PROPERTY(bool isAsync READ isAsync WRITE setAsync NOTIFY isAsyncChanged)

public:
    class Fetcher
    {
    public:
        explicit Fetcher(cv::VideoCapture& video);
        virtual ~Fetcher();
        void fetch();
        virtual cv::Mat get() = 0;

    protected:
        cv::VideoCapture& video_;
        cv::Mat image_;
    };

    class SyncFetcher : public Fetcher
    {
    public:
        explicit SyncFetcher(cv::VideoCapture& video);
        cv::Mat get() override;
    };

    class AsyncFetcher : public Fetcher
    {
    public:
        explicit AsyncFetcher(cv::VideoCapture& video);
        ~AsyncFetcher();
        cv::Mat get() override;
        void setFps(int fps) { fps_ = fps; }

    private:
        std::thread thread_;
        bool isRunning_;
        int fps_;
    };

    explicit Camera(QQuickItem *parent = nullptr);
    ~Camera();
    void paint(QPainter *painter) override;

    bool isAsync() const;
    void setAsync(bool isAsync);

    Q_INVOKABLE void open();
    Q_INVOKABLE void close();
    Q_INVOKABLE void fetch();
    Q_INVOKABLE bool isOpened() const;

private:
    cv::VideoCapture video_;
    int camera_;
    int fps_;
    std::shared_ptr<Fetcher> fetcher_;
    bool isAsync_;

signals:
    void fpsChanged() const;
    void deviceChanged() const;
    void isAsyncChanged() const;
};

}

#endif
