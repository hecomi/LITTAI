#ifndef DIFF_IMAGE_H
#define DIFF_IMAGE_H

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
    Q_PROPERTY(float sharpness MEMBER sharpness_ NOTIFY sharpnessChanged)
    Q_PROPERTY(double intensityCorrectionMin MEMBER intensityCorrectionMin_ NOTIFY intensityCorrectionMinChanged)
    Q_PROPERTY(double intensityCorrectionMax MEMBER intensityCorrectionMax_ NOTIFY intensityCorrectionMaxChanged)

public:
    explicit DiffImage(QQuickItem *parent = 0);

    void setBaseImage(const QVariant& image);
    QVariant baseImage() const;
    QVariant intensityCorrectionImage() const;
    void setInputImage(const QVariant& image);
    QVariant inputImage() const;

private:
    void unsharpMask(cv::Mat& image, float k);
    void createIntensityCorrectionImage();
    void applyIntensityCorrection(cv::Mat& image);

    cv::Mat baseImage_, inputImage_, intensityCorrectionImage_;
    double gamma_;
    float sharpness_;
    double intensityCorrectionMax_, intensityCorrectionMin_;

signals:
    void baseImageChanged() const;
    void inputImageChanged() const;
    void gammaChanged() const;
    void sharpnessChanged() const;
    void intensityCorrectionImageChanged() const;
    void intensityCorrectionMinChanged() const;
    void intensityCorrectionMaxChanged() const;
};


}

#endif // DIFF_IMAGE_H
