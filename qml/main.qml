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
    width: xtion.width || 640
    height: xtion.height || 480
    visible: true

    Storage {
        id: storage
        name: 'LITTAI'
        description: 'Interaction recognizer for LITTAI project.'
    }

    /*
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
    */

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

    /*
    Homography {
    Homography {
        id: homography
        image: xtion.image
        srcPoints: targetArea.points
        anchors.fill: parent

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
    */

    Analyzer {
        id: analyzer
        inputImage: xtion.image
        anchors.fill: parent
        onImageChanged: fpsCounter.update()
        threshold: storage.get('threshold') || 128
        onThresholdChanged: storage.set('threshold', threshold)

        Fps {
            id: fpsCounter
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.margins: 5
        }

        Text {
            id: info
            font.pointSize: 18
            color: '#fff'
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.rightMargin: font.pointSize
            anchors.bottomMargin: font.pointSize
            text: getText()
            function getText() {
                var text = '';
                text += 'THRESH: <font color="#0f0"><b>' + analyzer.threshold + '</b></font>';
                return text;
            }
        }

        focus: true
        Keys.onPressed: {
            switch (event.key) {
                case Qt.Key_Down:
                    analyzer.threshold -= 1;
                    break;
                case Qt.Key_Up:
                    analyzer.threshold += 1;
                    break;
                case Qt.Key_C:
                    console.log("get base image!");
                    baseImage = xtion.image;
                    break;
            }
        }
    }

    Tracker {
        id: tracker
        anchors.fill: parent
        inputImage: analyzer.image
        onInputImageChanged: track()
        templateImage: templateImage.image
        templateThreshold: 0.5
        onTemplateThresholdChanged: track()
        onItemsChanged: {
            items.forEach(function(item) {
                console.log(item.id, item.x, item.y, item.angle.toFixed(2));
            });
        }

        Image {
            id: templateImage
            filePath: '/Users/hecomi/Desktop/template_xtion.png'
        }

        Text {
            font.pointSize: 18
            color: '#fff'
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.rightMargin: font.pointSize
            anchors.bottomMargin: font.pointSize
            text: getText();
            function getText() {
                var text = '<font color="red">TH:</font> ' + parent.templateThreshold.toFixed(2);
                return text;
            }
        }

        focus: true
        Keys.onPressed: {
            switch (event.key) {
                case Qt.Key_A:
                    templateThreshold -= 0.01
                    break;
                case Qt.Key_S:
                    templateThreshold += 0.01
                    break;
                case Qt.Key_T:
                    tracker.track();
                    break;
            }
        }
    }

    Image {
        id: template
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        width: imageWidth / 2
        height: imageHeight / 2
        image: tracker.templateImage
    }
}
