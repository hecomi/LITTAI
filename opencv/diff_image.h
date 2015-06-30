#ifndef ANALYZER_H
#define ANALYZER_H

#include "image.h"


namespace Littai
{


class DiffImage : public Image
{
    Q_OBJECT
    Q_PROPERTY(QVariant baseImage WRITE setBaseImage READ baseImage NOTIFY baseImageChanged)
    Q_PROPERTY(QVariant inputImage WRITE setInputImage READ inputImage NOTIFY inputImageChanged)
    Q_PROPERTY(QVariant intensityCorrectionImage READ intensityCorrectionImage NOTIFY intensityCorrectionImageChanged)
    Q_PROPERTY(double gamma MEMBER gamma_ NOTIFY gammaChanged)
    Q_PROPERTY(double intensityPower MEMBER intensityPower_ NOTIFY intensityPowerChanged)

public:
    explicit DiffImage(QQuickItem *parent = 0);

    void setBaseImage(const QVariant& image);
    QVariant baseImage() const;
    QVariant intensityCorrectionImage() const;
    void setInputImage(const QVariant& image);
    QVariant inputImage() const;

private:
    void createIntensityCorrectionImage();
    void applyIntensityCorrection(cv::Mat& image);

    cv::Mat baseImage_, inputImage_, intensityCorrectionImage_;
    double gamma_;
    double intensityPower_;

signals:
    void baseImageChanged() const;
    void inputImageChanged() const;
    void gammaChanged() const;
    void intensityCorrectionImageChanged() const;
    void intensityPowerChanged() const;
};


}

#endif // ANALYZER_H
