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

        MarkerTracker {
            id: markerTracker

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.maximumWidth: parent.width / 2
            Layout.maximumHeight: parent.height

            fps: 30
            inputImage: window.diffImage
            onInputImageChanged: update()
            contrastThreshold: contrastSlider.value
            onImageChanged: markerTrackerFpsCounter.update()

            Fps {
                id: markerTrackerFpsCounter
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

        InputSlider {
            id: contrastSlider
            min: 0
            max: 60
            isInteger: true
            defaultValue: storage.get('markerTracker.contrastThreshold') || min
            onValueChanged: storage.set('markerTracker.contrastThreshold', value)
            label: 'Contrast Threshold'
        }
    }
}
