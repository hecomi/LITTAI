#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQuick>

#include "image.h"
#include "camera.h"
#include "xtion.h"
#include "homography.h"

using namespace Littai;



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qmlRegisterType<Image>("Littai", 1, 0, "Image");
    qmlRegisterType<Camera>("Littai", 1, 0, "Camera");
    qmlRegisterType<Xtion>("Littai", 1, 0, "Xtion");
    qmlRegisterType<Homography>("Littai", 1, 0, "Homography");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    return app.exec();
}
