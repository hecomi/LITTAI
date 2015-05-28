import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import Littai 1.0

ApplicationWindow {
    title: qsTr("Hello World")
    width: image.imageWidth
    height: image.imageHeight
    visible: true

    Image {
        id: image
        anchors.fill: parent
        filePath: "/Users/hecomi/Pictures/zzz.png"
    }
}
