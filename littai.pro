CONFIG += c++11

TEMPLATE = app

QT += qml quick widgets

SOURCES += main.cpp

RESOURCES += qml.qrc

QML_IMPORT_PATH =

QMAKE_CXXFLAGS_WARN_OFF += -Wno-overloaded-virtual

include(opencv/opencv.pri)
include(openni/openni.pri)
include(realsense/realsense.pri)
include(kinect_v2/kinect_v2.pri)
include(aruco/aruco.pri)
include(osc/osc.pri)
