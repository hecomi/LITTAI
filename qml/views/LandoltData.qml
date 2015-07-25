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
    property bool landoltTouched: false
    property double landoltTouchX: 0
    property double landoltTouchY: 0
    property int landoltTouchCount: 0
    property var landoltTouchImage: null

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
        id: touchImage
        anchors.left: image.right
        anchors.top: parent.top
        anchors.margins: 5
        width: 96
        height: 96
        color: '#000000'

        Image {
            image: landoltTouchImage
            width: (imageWidth > imageHeight) ? parent.width : parent.height * imageWidth / imageHeight
            height: (imageHeight > imageWidth) ? parent.height : parent.width * imageHeight / imageWidth
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    Rectangle {
        anchors.left: touchImage.right
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
            font.pointSize: 16
            lineHeight: 1.2
            function getText() {
                var text = '';
                text += '<font color="green">id:</font> <b><font color="red">' + landoltId + '</font></b>  ';
                text += '<font color="green">x:</font> <b><font color="red">' + landoltX.toFixed(2) + '</font></b>  ';
                text += '<font color="green">y:</font> <b><font color="red">' + landoltY.toFixed(2) + '</font></b>  ';
                text += '<font color="green">angle:</font> <b><font color="red">' + landoltAngle.toFixed(2) + '</font></b>  ';
                text += '<font color="green">size:</font> <b><font color="red">' + landoltSize.toFixed(2) + '</font></b>  ';
                text += '<br />';
                text += '<font color="green">touched:</font> <b><font color="red">' + landoltTouched + '</font></b>  ';
                text += '<font color="green">touchX:</font> <b><font color="red">' + landoltTouchX.toFixed(2) + '</font></b>  ';
                text += '<font color="green">touchY:</font> <b><font color="red">' + landoltTouchY.toFixed(2) + '</font></b>  ';
                text += '<font color="green">count:</font> <b><font color="red">' + landoltTouchCount + '</font></b>  ';
                text += '<br />';
                text += '<font color="green">frameCount:</font> <b><font color="red">' + frameCount + '</font></b>';
                return text;
            }
        }
    }
}
