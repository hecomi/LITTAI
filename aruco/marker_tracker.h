#ifndef MARKER_DETECTOR_H
#define MARKER_DETECTOR_H

#include <QVariantList>
#include <deque>
#include "image.h"


namespace Littai
{

class MarkerTracker : public Image
{
    Q_OBJECT
    Q_PROPERTY(QVariant inputImage WRITE setInputImage READ inputImage NOTIFY inputImageChanged)
    Q_PROPERTY(QString cameraParamsFilePath MEMBER cameraParamsFilePath_ NOTIFY cameraParamsFilePathChanged)
    Q_PROPERTY(int contrastThreshold MEMBER contrastThreshold_ NOTIFY contrastThresholdChanged)

public:
    explicit MarkerTracker(QQuickItem* parent = nullptr);

    void setInputImage(const QVariant& image);
    QVariant inputImage() const;

    void preProcess(cv::Mat& inputImage);
    void detectUsingAruco(cv::Mat& inputImage);
    Q_INVOKABLE void track();

private:
    mutable std::mutex mutex_;
    cv::Mat inputImage_;
    std::deque<cv::Mat> imageCaches_;
    QString cameraParamsFilePath_;
    int contrastThreshold_;

signals:
    void inputImageChanged() const;
    void cameraParamsFilePathChanged() const;
    void contrastThresholdChanged() const;
};

}

#endif // MARKER_DETECOTR_H
