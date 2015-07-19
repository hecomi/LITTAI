import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import Littai 1.0

import '../shapes'
import '../common'

ColumnLayout {

    function send(address, landolt) {
        window.osc.send(address, {
            id: landolt.id,
            x: landolt.x,
            y: landolt.y,
            angle: landolt.angle,
            radius: landolt.radius,
            width: landolt.width,
            height: landolt.height,
            frameCount: landolt.frameCount
        });
    }

    Storage {
        id: storage
        name: 'LITTAI'
        description: 'Interaction recognizer for LITTAI project.'
    }

    RowLayout {
        anchors.fill: parent

        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.minimumWidth: parent.width
        Layout.minimumHeight: parent.height - form.height - 5

        LandoltTracker {
            id: landoltTracker

            property var currentLandolts: ({})

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.maximumWidth: parent.width / 2
            Layout.maximumHeight: parent.height

            inputImage: window.diffImage
            fps: 30
            onInputImageChanged: update()
            templateImage: templateImage.image
            templateThreshold: templateThresholdSlider.value
            contrastThreshold: contrastThresholdSlider.value
            onContrastThresholdChanged: storage.set('contrastThreshold', contrastThreshold);
            onItemsChanged: {
                for (var id in currentLandolts) {
                    currentLandolts[id].checked = false;
                }
                items.forEach(function(landolt) {
                    if (currentLandolts[landolt.id]) {
                        results.updateLandolt(landolt);
                    } else {
                        results.createLandolt(landolt);
                    }
                    landolt.checked = true;
                    currentLandolts[landolt.id] = landolt;
                });
                for (var id in currentLandolts) {
                    var landolt = currentLandolts[id];
                    if (!landolt.checked) {
                        results.removeLandolt(landolt);
                        delete currentLandolts[id];
                    }
                }
            }
            onImageChanged: {
                landoltTrackerFpsCounter.update()
            }

            Image {
                id: templateImage
                //filePath: '/Users/hecomi/ProgramLocal/Qt/littai/img/template.png'
                filePath: 'D:/littai/img/template.png'
            }

            Fps {
                id: landoltTrackerFpsCounter
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.margins: 5
            }
        }

        GroupBox {
            property var landolts: ({})

            id: results
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.maximumWidth: parent.width / 2
            Layout.maximumHeight: parent.height

            ScrollView {
                anchors.fill: parent

                ColumnLayout {
                    id: resultArea
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }

            function createLandolt(landolt) {
                send('/landolt/create', landolt);
                var landoltDataQml = Qt.createComponent('LandoltData.qml');
                var landoltData = landoltDataQml.createObject(resultArea);
                landoltData.Layout.minimumWidth = width;
                landolts[landolt.id] = landoltData;
                updateLandolt(landolt, false);
            }

            function updateLandolt(landolt) {
                send('/landolt/update', landolt);
                if (landolt.id in landolts) {
                    var landoltData = landolts[landolt.id];
                    landoltData.landoltId = landolt.id;
                    landoltData.landoltX = landolt.x;
                    landoltData.landoltY = landolt.y;
                    landoltData.landoltAngle = landolt.angle * 180 / Math.PI;
                    landoltData.landoltSize = landolt.radius;
                    landoltData.landoltImage = landolt.image;
                    landoltData.frameCount = landolt.frameCount;
                }
            }

            function removeLandolt(landolt) {
                send('/landolt/remove', landolt);
                landolts[landolt.id].destroy();
                delete landolts[landolt.id];
            }
        }
    }

    GroupBox {
        id: form
        anchors.bottom: parent.bottom
        Layout.fillWidth: true

        RowLayout {
            spacing: 30

            InputSlider {
                id: templateThresholdSlider
                min: 0.0
                max: 1.0
                fixedLength: 2
                defaultValue: storage.get('landoltTracker.templateThreshold') || min
                onValueChanged: storage.set('landoltTracker.templateThreshold', value)
                label: 'Template Threshold'
            }

            InputSlider {
                id: contrastThresholdSlider
                min: 0
                max: 255
                isInteger: true
                defaultValue: storage.get('landoltTracker.contrastThreshold') || min
                onValueChanged: storage.set('landoltTracker.contrastThreshold', value)
                label: 'Contrast Threshold'
            }
        }
    }
}
