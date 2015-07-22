#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQuick>

#include "image.h"
#include "camera.h"
#include "homography.h"
#include "diff_image.h"
#include "reverse_image.h"
#include "landolt_tracker.h"
#include "xtion.h"
#include "realsense.h"
#include "kinect_v2.h"
#include "marker_tracker.h"
#include "osc_receiver.h"
#include "osc_sender.h"

using namespace Littai;



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qmlRegisterType<Image>("Littai", 1, 0, "Image");
    qmlRegisterType<Camera>("Littai", 1, 0, "Camera");
    qmlRegisterType<Homography>("Littai", 1, 0, "Homography");
    qmlRegisterType<DiffImage>("Littai", 1, 0, "DiffImage");
    qmlRegisterType<ReverseImage>("Littai", 1, 0, "ReverseImage");
    qmlRegisterType<LandoltTracker>("Littai", 1, 0, "LandoltTracker");
    qmlRegisterType<Xtion>("Littai", 1, 0, "Xtion");
    qmlRegisterType<RealSense>("Littai", 1, 0, "RealSense");
    qmlRegisterType<KinectV2>("Littai", 1, 0, "KinectV2");
    qmlRegisterType<MarkerTracker>("Littai", 1, 0, "MarkerTracker");
    qmlRegisterType<OSCReceiver>("Littai", 1, 0, "OscReceiver");
    qmlRegisterType<OSCSender>("Littai", 1, 0, "OscSender");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    return app.exec();
}
