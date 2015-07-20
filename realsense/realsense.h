#ifndef REALSENSE_H
#define REALSENSE_H

#include "image.h"
#include <pxcsensemanager.h>
#include <memory>
#include <thread>
#include <mutex>


namespace Littai
{


class RealSense : public Image
{
    Q_OBJECT
    Q_PROPERTY(int fps MEMBER fps_ NOTIFY fpsChanged)

public:
    explicit RealSense(QQuickItem *parent = nullptr);
    ~RealSense();
    Q_INVOKABLE void fetch();
    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();

private:
    void init();
    void asyncUpdate();
    void error(const QString& msg);

    std::shared_ptr<PXCSenseManager> senseManager_;
    cv::Mat irImage_;
    int fps_;

    std::thread thread_;
    mutable std::mutex mutex_;
    bool isRunning_;

signals:
    void fpsChanged() const;
};

}

#endif
