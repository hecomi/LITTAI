﻿import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import Littai 1.0

import '../shapes'
import '../common'

ColumnLayout {
    spacing: 10
    property string baseImagePath: 'D:/littai/img/baseImage.png'
    property string intensityCorrectionImagePath: 'D:/littai/img/intensityCorrectionImage.png'

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

        Image {
            id: inputImage
            image: window.homographyImage

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.maximumWidth: parent.width / 2
            Layout.maximumHeight: parent.height

            Rectangle {
                id: baseRect
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.rightMargin: 5
                anchors.bottomMargin: 5
                width: parent.width / 4
                height: parent.height / 4
                color: '#88000000'
                border.color: '#88ffffff'
                border.width: 1

                Image {
                    id: base
                    anchors.fill: parent
                    filePath: baseImagePath

                    Rectangle {
                        anchors.bottom: parent.bottom
                        width: parent.width
                        height: 16
                        color: '#aa000000'
                        border.color: '#88ffffff'
                        border.width: 1

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.verticalCenter: parent.verticalCenter
                            color: '#ffffff'
                            font.pointSize: 12
                            text: 'Base'
                        }
                    }
                }
            }

            Rectangle {
                id: baseIntensityRect
                anchors.right: parent.right
                anchors.bottom: baseRect.top
                anchors.rightMargin: 5
                anchors.bottomMargin: 5
                width: parent.width / 4
                height: parent.height / 4
                color: '#88000000'
                border.color: '#88ffffff'
                border.width: 1

                Image {
                    id: baseIntensity
                    anchors.fill: parent
                    filePath: intensityCorrectionImagePath

                    Rectangle {
                        anchors.bottom: parent.bottom
                        width: parent.width
                        height: 16
                        color: '#aa000000'
                        border.color: '#88ffffff'
                        border.width: 1

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.verticalCenter: parent.verticalCenter
                            color: '#ffffff'
                            font.pointSize: 12
                            text: 'Intensity Ratio'
                        }
                    }
                }
            }
        }

        DiffImage {
            id: diff
            gamma: gammaSlider.value
            sharpness: sharpnessSlider.value
            intensityCorrectionMin: intensityCorrectionMinSlider.value
            intensityCorrectionMax: intensityCorrectionMaxSlider.value
            inputImage: inputImage.image
            baseImage: base.image
            onImageChanged: {
                fpsCounter.update();
                window.diffImage = image;
            }

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

            Button {
                text: "Set Base Image As Current"
                onClicked: setBaseImage()
            }

            InputSlider {
                id: gammaSlider
                min: 0.0
                max: 3.0
                fixedLength: 2
                defaultValue: storage.get('diffImage.gamma') || min
                onValueChanged: storage.set('diffImage.gamma', value)
                label: 'Gamma Correction'
            }

            InputSlider {
                id: sharpnessSlider
                min: 0.0
                max: 10.0
                fixedLength: 2
                defaultValue: storage.get('diffImage.sharpness') || 1.0
                onValueChanged: storage.set('diffImage.sharpness', value)
                label: 'Sharpness'
            }

            InputSlider {
                id: intensityCorrectionMinSlider
                min: 0.0
                max: 255.0
                fixedLength: 0
                defaultValue: storage.get('diffImage.intensityCorrectionMin') || min
                onValueChanged: storage.set('diffImage.intensityCorrectionMin', value)
                label: 'Intensity Correction Min'
            }

            InputSlider {
                id: intensityCorrectionMaxSlider
                min: 0.0
                max: 255.0
                fixedLength: 0
                defaultValue: storage.get('diffImage.intensityCorrectionMax') || min
                onValueChanged: storage.set('diffImage.intensityCorrectionMax', value)
                label: 'Intensity Correction Max'
            }
        }
    }

    Item {
        Fps {
            id: fpsCounter
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.margins: 5
        }
    }

    function setBaseImage() {
        base.image = inputImage.image;
        baseIntensity.image = diff.intensityCorrectionImage;
        base.saveToFile(baseImagePath);
        baseIntensity.saveToFile(intensityCorrectionImagePath);
    }
}
