#ifndef TRACKER_H


#include "image.h"


namespace Littai
{


class Tracker : public Image
{
    Q_OBJECT
    Q_PROPERTY(QVariant inputImage WRITE setInputImage READ inputImage NOTIFY inputImageChanged)
    Q_PROPERTY(QVariant templateImage WRITE setTemplateImage READ templateImage NOTIFY templateImageChanged)
    Q_PROPERTY(QVariant result READ result NOTIFY resultChanged)
    Q_PROPERTY(double templateThreshold MEMBER templateThreshold_ NOTIFY templateThresholdChanged)

public:
    explicit Tracker(QQuickItem *parent = 0);

    void setInputImage(const QVariant& image);
    QVariant inputImage() const;
    void setTemplateImage(const QVariant& image);
    QVariant templateImage() const;
    Q_INVOKABLE void track();
    QVariant result();

private:
    cv::Mat inputImage_;
    cv::Mat templateImage_;
    std::vector<cv::Rect> result_;
    mutable std::mutex mutex_;
    double templateThreshold_;

signals:
    void inputImageChanged() const;
    void templateImageChanged() const;
    void resultChanged() const;
    void templateThresholdChanged() const;
};


}

#endif // HoughCircles_H
