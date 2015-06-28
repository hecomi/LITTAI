import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import Littai 1.0

import '../shapes'
import '../common'

ColumnLayout {

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
                items.forEach(function(item) {
                    console.log(item.id, item.x, item.y, item.angle.toFixed(2));
                });
            }
            onImageChanged: landoltTrackerFpsCounter.update()

            Image {
                id: templateImage
                filePath: '/Users/hecomi/ProgramLocal/Qt/littai/img/template.png'
            }

            Fps {
                id: landoltTrackerFpsCounter
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.margins: 5
            }
        }

        GroupBox {
            id: results
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.maximumWidth: parent.width / 2
            Layout.maximumHeight: parent.height
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
