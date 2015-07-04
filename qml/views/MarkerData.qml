import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import Littai 1.0

Item {
    property int markerId: 0
    property double markerX: 0
    property double markerY: 0
    property double markerSize: 0
    property double markerAngle: 0
    property int frameCount: 0
    property var markerImage: null
    property var markerPolygon: []
    property var markerEdges: []

    Layout.minimumHeight: image.height + 10
    Layout.fillWidth: true

    Rectangle {
        id: image
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 5
        width: 96
        height: 96
        color: '#000000'

        Image {
            image: markerImage
            width: (imageWidth > imageHeight) ? parent.width : parent.height * imageWidth / imageHeight
            height: (imageHeight > imageWidth) ? parent.height : parent.width * imageHeight / imageWidth
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    Rectangle {
        anchors.left: image.right
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 5
        height: parent.height - anchors.margins * 2
        border.color: '#aaaaaa'
        border.width: 1

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            text: getText();
            font.pointSize: 20
            lineHeight: 1.2
            function getText() {
                var text = '';
                text += '<font color="green">id:</font> <b><font color="red">' + markerId + '</font></b>  ';
                text += '<font color="green">x:</font> <b><font color="red">' + markerX.toFixed(2) + '</font></b>  ';
                text += '<font color="green">y:</font> <b><font color="red">' + markerY.toFixed(2) + '</font></b>  ';
                text += '<font color="green">frameCount:</font> <b><font color="red">' + frameCount + '</font></b>';
                text += '<br />';
                text += '<font color="green">angle:</font> <b><font color="red">' + markerAngle.toFixed(2) + '</font></b>  ';
                text += '<font color="green">size:</font> <b><font color="red">' + markerSize.toFixed(2) + '</font></b>  ';
                text += '<font color="green">polygon:</font> <b><font color="red">' + markerPolygon.length + '</font></b>  ';
                text += '<font color="green">edges:</font> <b><font color="red">' + markerEdges.length + '</font></b>  ';
                return text;
            }
        }
    }
}
