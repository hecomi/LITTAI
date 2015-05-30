#ifndef HOMOGRAPHY_H
#define HOMOGRAPHY_H

#include <QVariantList>
#include "image.h"


namespace Littai
{

class Homography : public Image
{
    Q_OBJECT
    Q_PROPERTY(QVariant image READ image WRITE setImage)
    Q_PROPERTY(QVariantList srcPoints MEMBER srcPoints_)

public:
    explicit Homography(QQuickItem* parent = nullptr);
    void setImage(const QVariant& image);

private:
    QVariantList srcPoints_;
};

}

#endif // HOMOGRAPHY_H
