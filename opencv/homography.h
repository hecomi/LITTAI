#ifndef HOMOGRAPHY_H
#define HOMOGRAPHY_H

#include <QVariantList>
#include "image.h"


namespace Littai
{

class Homography : public Image
{
    Q_OBJECT
    Q_PROPERTY(QVariant image READ image WRITE setImage NOTIFY imageChanged)
    Q_PROPERTY(QVariantList srcPoints MEMBER srcPoints_)
    Q_PROPERTY(int outputWidth MEMBER width_ NOTIFY widthChanged);
    Q_PROPERTY(int outputHeight MEMBER height_ NOTIFY heightChanged);

public:
    explicit Homography(QQuickItem* parent = nullptr);
    void setImage(const QVariant& image);

private:
    QVariantList srcPoints_;
    int width_, height_;

signals:
    void imageChanged() const;
    void widthChanged() const;
    void heightChanged() const;
};

}

#endif // HOMOGRAPHY_H
