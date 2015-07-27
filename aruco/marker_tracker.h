#ifndef MARKER_DETECTOR_H
#define MARKER_DETECTOR_H

#include <QVariantList>
#include <thread>
#include <deque>
#include "image.h"


namespace Littai
{

class TrackedEdge : public cv::Point
{
public:
    TrackedEdge(const cv::Point& point)
        : cv::Point(point)
        , frameCount(0)
        , lostCount(0)
        , checked(false)
        , activated(false)
    {}
    int id;
    cv::Point2d direction;
    int frameCount;
    int lostCount;
    bool checked;
    bool activated;

    static int GetId()
    {
        return currentId++;
    }

private:
    static int currentId;
};


struct TrackedPattern
{
    std::vector<int> edgeIndices;
    int pattern;
};


struct TrackedMarker
{

    unsigned int id;
    double x, y;
    double vx, vy;
    double px, py;
    std::chrono::system_clock::time_point t;
    double trackedX, trackedY;
    double angle;
    double trackedAngle;
    double size;
    int frameCount;
    int lostCount;
    bool checked;
    cv::Rect bound;
    std::vector<cv::Point> polygon;
    std::vector<int> indices;
    std::vector<TrackedEdge> edges;
    std::vector<TrackedPattern> patterns;
    cv::Mat image;

    TrackedMarker()
        : id(-1), x(0), y(0), vx(0), vy(0), px(0), py(0), trackedX(0), trackedY(0), angle(0), trackedAngle(-1)
        , frameCount(0), lostCount(0), checked(false)
    {
    }

    void update(const TrackedMarker& other)
    {
        x     = other.x;
        y     = other.y;
        angle = other.angle;
        size  = other.size;
    }

    void print()
    {
        qDebug("id: %d  x: %.2f  y: %.2f  angle: %.2f  frame: %d",
            id, x, y, angle, frameCount);
    }
};


class MarkerTracker : public Image
{
    Q_OBJECT
    Q_PROPERTY(QVariant inputImage WRITE setInputImage READ inputImage NOTIFY inputImageChanged)
    Q_PROPERTY(int contrastThreshold MEMBER contrastThreshold_ NOTIFY contrastThresholdChanged)
    Q_PROPERTY(int contrastThreshold2 MEMBER contrastThreshold2_ NOTIFY contrastThreshold2Changed)
    Q_PROPERTY(int contrastThreshold3 MEMBER contrastThreshold3_ NOTIFY contrastThreshold3Changed)
    Q_PROPERTY(int fps MEMBER fps_ NOTIFY fpsChanged)
    Q_PROPERTY(int predictionFrame MEMBER predictionFrame_ NOTIFY predictionFrameChanged)
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
    void detectMotions(cv::Mat& resultImage, cv::Mat& inputImage);
    void detectPatterns(cv::Mat& resultImage, cv::Mat& inputImage);
    void predictPosition();
    std::vector<int> triangulatePolygons(const std::vector<cv::Point>& polygon);

    std::thread thread_;
    mutable std::mutex mutex_;
    bool isFinished_;
    bool isImageUpdated_;

    cv::Mat inputImage_;
    std::deque<cv::Mat> imageCaches_;
    cv::Mat historyImage_;
    cv::Mat preImage_;
    const std::chrono::system_clock::time_point startTime_;

    int contrastThreshold_, contrastThreshold2_, contrastThreshold3_;
    int fps_;
    int predictionFrame_;

    std::list<TrackedMarker> markers_;

signals:
    void inputImageChanged() const;
    void cameraParamsFilePathChanged() const;
    void contrastThresholdChanged() const;
    void contrastThreshold2Changed() const;
    void contrastThreshold3Changed() const;
    void fpsChanged() const;
    void markersChanged() const;
    void predictionFrameChanged() const;
};

}

#endif // MARKER_DETECOTR_H
