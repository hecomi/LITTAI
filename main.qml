import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import Littai 1.0

ApplicationWindow {
    title: "Hello World"
    width: image.imageWidth
    height: image.imageHeight
    visible: true

    Camera {
        id: image
        anchors.fill: parent
        filePath: "/Users/hecomi/Pictures/zzz.png"
        camera: 0
        fps: 30

        Component.onCompleted: open()
        onError: console.log(error)

        Timer {
            interval: 1000 / parent.fps
            running: true
            repeat: true
            onTriggered: parent.update();
        }
    }
}
