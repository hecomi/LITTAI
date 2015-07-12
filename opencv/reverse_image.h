#ifndef REVERSE_IMAGE_H
#define REVERSE_IMAGE_H

#include "image.h"


namespace Littai
{


class ReverseImage : public Image
{
    Q_OBJECT
    Q_PROPERTY(QVariant image READ image WRITE setImage NOTIFY imageChanged)
    Q_PROPERTY(bool horizontal MEMBER horizontal_ NOTIFY horizontalChanged)
    Q_PROPERTY(bool vertical MEMBER vertical_ NOTIFY verticalChanged)

public:
    explicit ReverseImage(QQuickItem *parent = 0);
    QVariant image() const;
    void setImage(const QVariant& image);

private:
    mutable std::mutex mutex_;
    bool horizontal_;
    bool vertical_;

signals:
    void imageChanged() const;
    void horizontalChanged() const;
    void verticalChanged() const;
};


}

#endif // REVERSE_IMAGE_H
