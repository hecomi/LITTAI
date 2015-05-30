#ifndef CAMERA_H
#define CAMERA_H

#include "image.h"

namespace Littai
{

class Camera : public Image
{
    Q_OBJECT
    Q_PROPERTY(int fps MEMBER fps_ NOTIFY fpsChanged)
    Q_PROPERTY(int camera MEMBER camera_ NOTIFY deviceChanged)

public:
    explicit Camera(QQuickItem *parent = nullptr);
    ~Camera();
    void paint(QPainter *painter) override;

    Q_INVOKABLE void open();
    Q_INVOKABLE void close();
    Q_INVOKABLE bool isOpened() const;

private:
    cv::VideoCapture video_;
    int camera_;
    int fps_;

signals:
    void fpsChanged() const;
    void deviceChanged() const;
};

}

#endif
