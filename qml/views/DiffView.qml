import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import Littai 1.0

import '../shapes'
import '../common'

ColumnLayout {
    spacing: 10

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
                            text: 'base image'
                        }
                    }
                }
            }
        }

        DiffImage {
            id: diff
            inputImage: inputImage.image
            baseImage: base.image
            onImageChanged: window.diffImage = image

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
            Button {
                text: "Set Base Image As Current"
                onClicked: setBaseImage()
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
    }
}
