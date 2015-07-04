import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import Littai 1.0

Item {
    property int landoltId: 0
    property double landoltX: 0
    property double landoltY: 0
    property double landoltSize: 0
    property double landoltAngle: 0
    property int frameCount: 0
    property var landoltImage: null

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
            image: landoltImage
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
                text += '<font color="green">id:</font> <b><font color="red">' + landoltId + '</font></b>  ';
                text += '<font color="green">x:</font> <b><font color="red">' + landoltX.toFixed(2) + '</font></b>  ';
                text += '<font color="green">y:</font> <b><font color="red">' + landoltY.toFixed(2) + '</font></b>  ';
                text += '<font color="green">frameCount:</font> <b><font color="red">' + frameCount + '</font></b>';
                text += '<br />';
                text += '<font color="green">angle:</font> <b><font color="red">' + landoltAngle.toFixed(2) + '</font></b>  ';
                text += '<font color="green">size:</font> <b><font color="red">' + landoltSize.toFixed(2) + '</font></b>  ';
                return text;
            }
        }
    }
}
