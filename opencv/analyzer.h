#ifndef ANALYZER_H
#define ANALYZER_H

#include "image.h"


namespace Littai
{


class Analyzer : public Image
{
    Q_OBJECT
    Q_PROPERTY(int threshold MEMBER threshold_ NOTIFY thresholdChanged)
    Q_PROPERTY(QVariant baseImage WRITE setBaseImage READ baseImage NOTIFY baseImageChanged)
    Q_PROPERTY(QVariant inputImage WRITE setInputImage READ inputImage NOTIFY inputImageChanged)

public:
    explicit Analyzer(QQuickItem *parent = 0);

    void setBaseImage(const QVariant& image);
    QVariant baseImage() const;
    void setInputImage(const QVariant& image);
    QVariant inputImage() const;

private:
    int threshold_;
    cv::Mat baseImage_, inputImage_;

signals:
    void thresholdChanged() const;
    void baseImageChanged() const;
    void inputImageChanged() const;
};


}

#endif // ANALYZER_H
