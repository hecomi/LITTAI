CONFIG += c++11

TEMPLATE = app

QT += qml quick widgets

SOURCES += main.cpp

RESOURCES += qml.qrc

QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)
include(opencv/opencv.pri)
