#ifndef MARKER_DETECTOR_H
#define MARKER_DETECTOR_H

#include <QVariantList>
#include <thread>
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
    std::vector<cv::Point> polygon;
    std::vector<cv::Point> edges;
    cv::Mat image;

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
    Q_PROPERTY(int contrastThreshold MEMBER contrastThreshold_ NOTIFY contrastThresholdChanged)
    Q_PROPERTY(int fps MEMBER fps_ NOTIFY fpsChanged)
    Q_PROPERTY(QVariantList markers READ markers NOTIFY markersChanged)

public:
    explicit MarkerTracker(QQuickItem* parent = nullptr);
    ~MarkerTracker();

    void setInputImage(const QVariant& image);
    QVariant inputImage() const;
    QVariantList markers() const;

private:
    void track();
    void preProcess(cv::Mat& image);
    void detectMarkers(cv::Mat& resultImage, cv::Mat& inputImage);
    void detectPolygons(cv::Mat& resultImage, cv::Mat& inputImage);

    std::thread thread_;
    mutable std::mutex mutex_;
    bool isFinished_;
    bool isImageUpdated_;

    cv::Mat inputImage_;
    std::deque<cv::Mat> imageCaches_;

    int contrastThreshold_;
    int fps_;

    std::list<TrackedMarker> markers_;

signals:
    void inputImageChanged() const;
    void cameraParamsFilePathChanged() const;
    void contrastThresholdChanged() const;
    void fpsChanged() const;
    void markersChanged() const;
};

}

#endif // MARKER_DETECOTR_H
