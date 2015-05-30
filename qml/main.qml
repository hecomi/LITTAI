import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import Littai 1.0
import 'Shapes'

ApplicationWindow {
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

    Homography {
        image: camera.image
        anchors.fill: parent
        srcPoints: targetArea.points

        DeformableBox {
            id: targetArea
            anchors.fill: parent
            normalizeWidth: parent.width
            normalizeHeight: parent.height

            leftTopX: 10
            leftTopY: 10
            rightTopX: 100 - 10
            rightTopY: 10
            rightBottomX: 100 - 10
            rightBottomY: 100 - 10
            leftBottomX: 10
            leftBottomY: 100 - 10
        }
    }
}
