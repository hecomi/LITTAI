#ifndef REVERSE_IMAGE_H
#define REVERSE_IMAGE_H

#include "image.h"


namespace Littai
{


class ReverseImage : public Image
{
    Q_OBJECT
    Q_PROPERTY(QVariant inputImage WRITE setInputImage READ inputImage NOTIFY inputImageChanged)
    Q_PROPERTY(bool horizontal MEMBER horizontal_ NOTIFY horizontalChanged)
    Q_PROPERTY(bool vertical MEMBER vertical_ NOTIFY verticalChanged)

public:
    explicit ReverseImage(QQuickItem *parent = 0);
    void setInputImage(const QVariant& image);
    QVariant inputImage() const;

private:
    mutable std::mutex mutex_;
    cv::Mat inputImage_;
    bool horizontal_;
    bool vertical_;

signals:
    void inputImageChanged() const;
    void horizontalChanged() const;
    void verticalChanged() const;
};


}

#endif // REVERSE_IMAGE_H
