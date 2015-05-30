#ifndef XTION_H
#define XTION_H

#include <ni2/OpenNI.h>

#include "image.h"


namespace Littai
{


class ImageListener : public openni::VideoStream::NewFrameListener
{
public:
    const cv::Mat& getImage() const;

protected:
    cv::Mat image_;
    mutable std::mutex mutex_;
};


// ---


class IRImageListener : public ImageListener
{
private:
    void onNewFrame(openni::VideoStream& stream) override;
};


// ---


class ColorImageListener : public ImageListener
{
private:
    void onNewFrame(openni::VideoStream& stream) override;
};


// ---


class Xtion : public Image
{
    Q_OBJECT
    Q_PROPERTY(bool isColor MEMBER isColor_)

public:
    explicit Xtion(QQuickItem *parent = nullptr);
    ~Xtion();

    void initialize();
    void shutdown();
    Q_INVOKABLE bool isOpened() const;

    void paint(QPainter *painter) override;

private:
    static int  instanceNum;
    static bool isDeviceOpened;

    static std::shared_ptr<openni::Device>      device;
    static std::shared_ptr<openni::VideoStream> irStream;
    static std::shared_ptr<IRImageListener>     irListener;
    static std::shared_ptr<openni::VideoStream> colorStream;
    static std::shared_ptr<ColorImageListener>  colorListener;

    bool isColor_;

signals:
    void open() const;
    void close() const;
};


}

#endif // XTION_H
