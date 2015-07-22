#ifndef KINECT_V2_H
#define KINECT_V2_H

#include "image.h"
#include <basetyps.h>
#include <wtypes.h>
#include <Kinect.h>
#include <memory>
#include <thread>
#include <mutex>


namespace Littai
{


class KinectV2 : public Image
{
    Q_OBJECT
    Q_PROPERTY(int fps MEMBER fps_ NOTIFY fpsChanged)

public:
    explicit KinectV2(QQuickItem *parent = nullptr);
    ~KinectV2();
    Q_INVOKABLE void fetch();

private:
    void init();
    bool errorCheck(HRESULT result);

    std::shared_ptr<IKinectSensor> kinect_;
    std::shared_ptr<IInfraredFrameSource> irSource_;
    std::shared_ptr<IInfraredFrameReader> irReader_;
    int width_, height_;
    int fps_;
    std::vector<UINT16> data_;
    bool isInitialized_;

signals:
    void fpsChanged() const;
};

}

#endif
