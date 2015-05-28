#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQuick>

#include "opencv_image.h"

using namespace Littai;



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qmlRegisterType<Image>("Littai", 1, 0, "Image");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
