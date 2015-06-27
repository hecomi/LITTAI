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
    width: (640 + 480)
    height: 480
    visible: true

    Storage {
        id: storage
        name: 'LITTAI'
        description: 'Interaction recognizer for LITTAI project.'
    }

    Rectangle {
        id: leftPane
        anchors.left: parent.left
        anchors.top: parent.top
        width: 640
        height: parent.height

        Xtion {
            id: xtion
            anchors.fill: parent
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

        Homography {
            id: homography
            image: xtion.image
            srcPoints: targetArea.points
            outputWidth: 480
            outputHeight: 480
        }
    }

    Rectangle {
        id: rightPane
        anchors.left: leftPane.right
        anchors.top: parent.top
        width: parent.height
        height: parent.height

        DiffImage {
            id: analyzer
            inputImage: homography.image
            anchors.fill: parent
            onImageChanged: fpsCounter.update()
        }

        /*
        LandoltTracker {
            id: tracker
            anchors.fill: parent
            inputImage: analyzer.image
            onInputImageChanged: track()
            templateImage: templateImage.image
            templateThreshold: storage.get('templateThreshold') || 0.5
            onTemplateThresholdChanged: track();
            contrastThreshold: storage.get('contrastThreshold') || 128
            onContrastThresholdChanged: storage.set('contrastThreshold', contrastThreshold);
            onItemsChanged: {
                items.forEach(function(item) {
                    console.log(item.id, item.x, item.y, item.angle.toFixed(2));
                });
            }

            Image {
                id: templateImage
                filePath: '/Users/hecomi/Desktop/template_xtion.png'
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
        */

        MarkerTracker {
            id: markerTracker
            // anchors.bottom: template.top
            // anchors.left: parent.left
            // width: 200
            // height: width * imageHeight / imageWidth
            anchors.fill: parent
            inputImage: analyzer.image
            onInputImageChanged: track()
            cameraParamsFilePath: "/Users/hecomi/ProgramLocal/Qt/littai/aruco/intrinsics.yml"
            contrastThreshold: storage.get("markerTracker.contrastThreshold") || 100
            onContrastThresholdChanged: storage.set("markerTracker.contrastThreshold", contrastThreshold)

            Fps {
                id: fpsCounter
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.margins: 5
            }
        }
    }

    Item {
        anchors.fill: parent
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
                    analyzer.baseImage = homography.image;
                    break;
                case Qt.Key_A:
                    tracker.templateThreshold -= 0.01
                    break;
                case Qt.Key_D:
                    tracker.templateThreshold += 0.01
                    break;
                case Qt.Key_W:
                    tracker.contrastThreshold += 1
                    break;
                case Qt.Key_S:
                    tracker.contrastThreshold -= 1
                    break;
                case Qt.Key_T:
                    tracker.track();
                    break;
                case Qt.Key_1:
                    markerTracker.contrastThreshold += 1
                    break;
                case Qt.Key_2:
                    markerTracker.contrastThreshold -= 1
                    break;
            }
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
                var text = '';
                // text += '<font color="red">TEMP_TH:</font> ' + tracker.templateThreshold.toFixed(2) + '  ';
                // text += '<font color="red">CONT_TH:</font> ' + tracker.contrastThreshold + '  ';
                text += '<font color="red">MKTK_CONT_TH:</font>: <font color="green">' + markerTracker.contrastThreshold + '</font>  ';
                return text;
            }
        }
    }
}
