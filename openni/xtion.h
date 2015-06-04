#ifndef XTION_H
#define XTION_H

#include <ni2/OpenNI.h>
#include <mutex>
#include "image.h"


namespace Littai
{


class ImageListener : public openni::VideoStream::NewFrameListener
{
public:
    ImageListener(
        const std::shared_ptr<openni::Device>& device,
        openni::SensorType sensor);
    virtual ~ImageListener();
    virtual cv::Mat getImage() const;
    void setMode(int width, int height, int fps);
    void start();
    void stop();
    bool isStarted() const;

protected:
    std::shared_ptr<openni::VideoStream> stream_;
    cv::Mat image_;
    bool isStarted_;
    mutable std::mutex mutex_;
};


// ---


class IrImageListener : public ImageListener
{
public:
    explicit IrImageListener(const std::shared_ptr<openni::Device>& device);
    cv::Mat getImage() const override;

private:
    void onNewFrame(openni::VideoStream& stream) override;
};


// ---


class ColorImageListener : public ImageListener
{
public:
    explicit ColorImageListener(const std::shared_ptr<openni::Device>& device);
    cv::Mat getImage() const override;

private:
    void onNewFrame(openni::VideoStream& stream) override;
};


// ---


class Xtion : public Image
{
    Q_OBJECT
    Q_PROPERTY(SensorType sensorType READ sensorType WRITE setSensorType NOTIFY sensorTypeChanged)
    Q_PROPERTY(int imageWidth READ imageWidth WRITE setImageWidth NOTIFY imageWidthChanged)
    Q_PROPERTY(int imageHeight READ imageHeight WRITE setImageHeight NOTIFY imageHeightChanged)
    Q_PROPERTY(int fps READ fps WRITE setFps NOTIFY fpsChanged)

public:
    enum class SensorType
    {
        None  = -1,
        Ir    = openni::SENSOR_IR,
        Color = openni::SENSOR_COLOR,
        Depth = openni::SENSOR_DEPTH
    };
    Q_ENUMS(SensorType)

    explicit Xtion(QQuickItem *parent = nullptr);
    ~Xtion();
    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void restart();
    Q_INVOKABLE bool isOpened() const;
    Q_INVOKABLE void fetch();

    SensorType sensorType() const;
    void setSensorType(SensorType sensorType);
    int imageWidth() const;
    void setImageWidth(int width);
    int imageHeight() const;
    void setImageHeight(int height);
    int fps() const;
    void setFps(int fps);
    void updateMode();

    void paint(QPainter *painter) override;

private:
    static void initialize();
    static void shutdown();

    static std::shared_ptr<openni::Device> device;
    static int  instanceNum;
    static bool isDeviceOpened;

    std::shared_ptr<ImageListener> listener_;
    SensorType sensorType_;
    int imageWidth_, imageHeight_;
    int fps_;

signals:
    void sensorTypeChanged() const;
    void imageWidthChanged() const;
    void imageHeightChanged() const;
    void fpsChanged() const;
};


}

#endif // XTION_H
