import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import Littai 1.0

import '../shapes'
import '../common'

ColumnLayout {
    property var outputImage: null

    Storage {
        id: storage
        name: 'LITTAI'
        description: 'Interaction recognizer for LITTAI project.'
    }

    RowLayout {
        anchors.fill: parent

        ReverseImage {
            id: reversed
            image: kinect.image
            horizontal: false
            vertical: false
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.maximumWidth: parent.width * 4 / 7
            Layout.maximumHeight: parent.width * 3 / 7

            /*
            Xtion {
                id: xtion
                width: 1
                height: 1
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
            */

            /*
            RealSense {
                id: realsense
                width: 1
                height: 1
                fps: 60
                Component.onCompleted: start()
                Timer {
                    interval: 1000 / parent.fps
                    running: true
                    repeat: true
                    onTriggered: parent.fetch();
                }
            }
            */

            KinectV2 {
                id: kinect
                width: 1
                height: 1
                fps: 30
                Timer {
                    interval: 1000 / parent.fps
                    running: true
                    repeat: true
                    onTriggered: parent.fetch();
                }
            }

            DeformableBox {
                id: targetArea
                anchors.fill: parent

                leftTopX: storage.get('homo-leftTopX') || 0.1
                leftTopY: storage.get('homo-leftTopY') || 0.1
                rightTopX: storage.get('homo-rightTopX') || 0.9
                rightTopY: storage.get('homo-rightTopY') || 0.1
                rightBottomX: storage.get('homo-rightBottomX') || 0.9
                rightBottomY: storage.get('homo-rightBottomY') || 0.9
                leftBottomX: storage.get('homo-leftBottomX') || 0.1
                leftBottomY: storage.get('homo-leftBottomY') || 0.9

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

        Homography {
            id: homography
            image: reversed.image
            onImageChanged: window.homographyImage = image
            srcPoints: targetArea.points
            outputWidth: 480
            outputHeight: 480

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.maximumWidth: parent.width * 3 / 7
            Layout.maximumHeight: parent.width * 3 / 7
        }
    }

    Item {
        anchors.fill: parent

        Fps {
            id: fpsCounter
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.margins: 5
        }
    }
}
