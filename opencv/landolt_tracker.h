#ifndef LANDOLT_TRACKER_H
#define LANDOLT_TRACKER_H

#include "image.h"
#include <thread>
#include <list>


namespace Littai
{

struct TrackedItem
{
    unsigned int id;
    double x, y;
    double width, height;
    double radius;
    double angle;
    int frameCount;
    bool checked;
    cv::Mat roi;
    cv::Mat roiBase;
    cv::Mat image;

    TrackedItem()
        : id(-1), x(0), y(0), width(0), height(0), radius(0), angle(0)
        , frameCount(0), checked(false)
    {
    }

    static unsigned int GetId()
    {
        return currentId++;
    }

private:
    static int currentId;
};


class LandoltTracker : public Image
{
    Q_OBJECT
    Q_PROPERTY(QVariant inputImage WRITE setInputImage READ inputImage NOTIFY inputImageChanged)
    Q_PROPERTY(QVariant templateImage WRITE setTemplateImage READ templateImage NOTIFY templateImageChanged)
    Q_PROPERTY(QVariantList items READ items NOTIFY itemsChanged)
    Q_PROPERTY(int contrastThreshold MEMBER contrastThreshold_ NOTIFY contrastThresholdChanged)
    Q_PROPERTY(double templateThreshold MEMBER templateThreshold_ NOTIFY templateThresholdChanged)
    Q_PROPERTY(bool isOutputImage MEMBER isOutputImage_ NOTIFY isOutputImageChanged)
    Q_PROPERTY(int fps MEMBER fps_ NOTIFY fpsChanged)

public:
    explicit LandoltTracker(QQuickItem *parent = 0);
    ~LandoltTracker();
    void updateItems(const std::vector<TrackedItem>& currentItems);

    void setInputImage(const QVariant& image);
    QVariant inputImage() const;
    void setTemplateImage(const QVariant& image);
    QVariant templateImage() const;
    QVariantList items();

private:
    void track();
    void preProcess(cv::Mat& image);
    void detectLandolt(cv::Mat& outputImage, cv::Mat& inputImage);
    void detectLandoltTouch(cv::Mat& outputImage, cv::Mat& inputImage);

    std::thread thread_;
    mutable std::mutex mutex_;
    bool isFinished_;

    bool isOutputImage_;

    cv::Mat inputImage_;
    cv::Mat templateImage_;
    bool isImageUpdated_;

    int contrastThreshold_;
    double templateThreshold_;
    double radius_;
    int fps_;

    std::vector<TrackedItem> items_;

signals:
    void inputImageChanged() const;
    void templateImageChanged() const;
    void templateThresholdChanged() const;
    void contrastThresholdChanged() const;
    void isOutputImageChanged() const;
    void itemsChanged() const;
    void fpsChanged() const;
};


}

#endif // LANDOLT_TRACKER_H
