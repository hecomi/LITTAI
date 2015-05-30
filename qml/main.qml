import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import Littai 1.0
import 'Shapes'

ApplicationWindow {
    id: window
    title: 'Hello World'
    width: 640
    height: 480
    visible: true

    Camera {
        id: camera
        camera: 0
        fps: 60
        isAsync: true

        Component.onCompleted: open()
        onError: console.log(error)

        Timer {
            interval: 1000 / parent.fps
            running: true
            repeat: true
            onTriggered: parent.fetch();
        }
    }

    Xtion {
        id: xtion
        imageWidth: 640
        imageHeight: 480
        fps: 30
        sensorType: Xtion.Ir

        Component.onCompleted: start()

        Timer {
            interval: 1000 / parent.fps
            running: true
            repeat: true
            onTriggered: parent.fetch();
        }
    }

    Homography {
        image: xtion.image
        anchors.fill: parent
        srcPoints: targetArea.points

        DeformableBox {
            id: targetArea
            anchors.fill: parent
            normalizeWidth: parent.width
            normalizeHeight: parent.height

            leftTopX: 10
            leftTopY: 10
            rightTopX: window.width - 10
            rightTopY: 10
            rightBottomX: window.width - 10
            rightBottomY: window.height - 10
            leftBottomX: 10
            leftBottomY: window.height - 10
        }
    }
}
