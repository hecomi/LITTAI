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


class KinectV2FrameWaitWorker : public QObject
{
    Q_OBJECT

public:
    KinectV2FrameWaitWorker();
    ~KinectV2FrameWaitWorker();
    void setIrReader(IInfraredFrameReader* reader);

public slots:
    void start();
    void stop();

private:
    IInfraredFrameReader* irReader_;
    WAITABLE_HANDLE handle_;
    bool isRunning_;

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
    void fetch();
    bool errorCheck(HRESULT result);

    std::shared_ptr<KinectV2FrameWaitWorker> worker_;
    QThread workerThread_;

    std::shared_ptr<IKinectSensor> kinect_;
    std::shared_ptr<IInfraredFrameSource> irSource_;
    std::shared_ptr<IInfraredFrameReader> irReader_;
    int width_, height_;
    int fps_;
    std::vector<UINT16> data_;
    bool isInitialized_;

signals:
    void fpsChanged() const;
    void waitStart();
    void waitStop();
};

}

#endif
