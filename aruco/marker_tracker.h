#ifndef MARKER_DETECTOR_H
#define MARKER_DETECTOR_H

#include <QVariantList>
#include <deque>
#include "image.h"


namespace Littai
{


struct TrackedMarker
{
    unsigned int id;
    double rawX, rawY;
    double x, y;
    double angle;
    double size;
    int frameCount;
    int lostCount;
    bool checked;
    std::vector<cv::Point> contour;

    TrackedMarker()
        : id(-1), rawX(0), rawY(0), x(0), y(0), angle(0)
        , frameCount(0), lostCount(0), checked(false)
    {
    }

    void update(const TrackedMarker& other)
    {
        x     = other.x;
        y     = other.y;
        rawX  = other.rawX;
        rawY  = other.rawY;
        angle = other.angle;
        size  = other.size;
    }

    void print()
    {
        qDebug("id: %d  x: %.2f  y: %.2f  angle: %.2f  frame: %d  rawX: %.0f  rawY: %.0f",
            id, x, y, angle, frameCount, rawX, rawY);
    }

private:
    static int currentId;
};


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
    std::vector<TrackedMarker> detectUsingAruco(cv::Mat& inputImage);
    Q_INVOKABLE void track();

private:
    mutable std::mutex mutex_;
    cv::Mat inputImage_;
    std::deque<cv::Mat> imageCaches_;
    QString cameraParamsFilePath_;
    int contrastThreshold_;
    std::list<TrackedMarker> markers_;

signals:
    void inputImageChanged() const;
    void cameraParamsFilePathChanged() const;
    void contrastThresholdChanged() const;
};

}

#endif // MARKER_DETECOTR_H
