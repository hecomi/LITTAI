#ifndef KINECT_V2_H
#define KINECT_V2_H

#include "image.h"
#include <basetyps.h>
#include <wtypes.h>
#include <Kinect.h>
#include <memory>
#include <QThread>


namespace Littai
{


class KinectV2FrameReadWorker : public QObject
{
    Q_OBJECT

public:
    KinectV2FrameReadWorker();
    ~KinectV2FrameReadWorker();
    void setIrReader(IInfraredFrameReader* reader);
    void setSize(int width, int height);
    const cv::Mat& getImage() const;

public slots:
    void start();
    void stop();

private:
    int width_, height_;
    std::vector<UINT16> data_;
    cv::Mat image_;
    IInfraredFrameReader* irReader_;
    WAITABLE_HANDLE handle_;
    bool isRunning_;
    mutable std::mutex mutex_;

signals:
    void newFrameArrived() const;
};


class KinectV2 : public Image
{
    Q_OBJECT
    Q_PROPERTY(int fps MEMBER fps_ NOTIFY fpsChanged)

public:
    explicit KinectV2(QQuickItem *parent = nullptr);
    ~KinectV2();

private:
    void init();
    void start();
    void stop();
    bool errorCheck(HRESULT result);

    KinectV2FrameReadWorker worker_;
    QThread workerThread_;

    std::shared_ptr<IKinectSensor> kinect_;
    std::shared_ptr<IInfraredFrameSource> irSource_;
    std::shared_ptr<IInfraredFrameReader> irReader_;
    int width_, height_;
    int fps_;
    bool isInitialized_;

private slots:
    void fetch();

signals:
    void fpsChanged() const;
    void waitStart();
    void waitStop();
};

}

#endif
