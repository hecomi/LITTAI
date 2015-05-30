import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import Littai 1.0
import 'Shapes'
import 'Common'

ApplicationWindow {
    id: window
    title: 'Hello World'
    width: 640
    height: 480
    visible: true

    Storage {
        id: storage
        name: 'LITTAI'
        description: 'Interaction recognizer for LITTAI project.'
    }

    Camera {
        id: camera
        camera: 0
        fps: 30
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
        sensorType: Xtion.Color

        Component.onCompleted: start()

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
        onImageChanged: fpsCounter.update()

        Fps {
            id: fpsCounter
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.margins: 5
        }

        DeformableBox {
            id: targetArea
            anchors.fill: parent
            normalizeWidth: parent.width
            normalizeHeight: parent.height

            leftTopX: storage.get('homo-leftTopX') || 10
            leftTopY: storage.get('homo-leftTopY') || 10
            rightTopX: storage.get('homo-rightTopX') || normalizeWidth - 10
            rightTopY: storage.get('homo-rightTopY') || 10
            rightBottomX: storage.get('homo-rightBottomX') || normalizeWidth - 10
            rightBottomY: storage.get('homo-rightBottomY') || normalizeHeight - 10
            leftBottomX: storage.get('homo-leftBottomX') || 10
            leftBottomY: storage.get('homo-leftBottomY') || normalizeHeight - 10

            onLeftTopXChanged: storage.set('homo-leftTopX', leftTopX)
            onLeftTopYChanged: storage.set('homo-leftTopY', leftTopY)
            onRightTopXChanged: storage.set('homo-rightTopX', rightTopX)
            onRightTopYChanged: storage.set('homo-rightTopY', rightTopY)
            onRightBottomXChanged: storage.set('homo-rightBottomX', rightBottomX)
            onRightBottomYChanged: storage.set('homo-rightBottomY', rightBottomY)
            onLeftBottomXChanged: storage.set('homo-leftBottomX', leftBottomX)
            onLeftBottomYChanged: storage.set('homo-leftBottomY', leftBottomY)
        }
    }
}
