import QtQuick 2.0

Item {
    property real leftTopX: 0.1
    property real leftTopY: 0.1
    property real leftBottomX: 0.1
    property real leftBottomY: 0.9
    property real rightTopX: 0.9
    property real rightTopY: 0.1
    property real rightBottomX: 0.9
    property real rightBottomY: 0.9

    property real pointSize: 10
    property color color: "#ff0000"

    property variant points: [
        [leftTopX,     leftTopY],
        [rightTopX,    rightTopY],
        [rightBottomX, rightBottomY],
        [leftBottomX,  leftBottomY]
    ]

    Component.onCompleted: changeScale()
    onWidthChanged: changeScale()
    onHeightChanged: changeScale()

    function changeScale() {
        leftTop.x = leftTopX * parent.width;
        leftTop.y = leftTopY * parent.height;
        rightTop.x = rightTopX * parent.width;
        rightTop.y = rightTopY * parent.height;
        rightBottom.x = rightBottomX * parent.width;
        rightBottom.y = rightBottomY * parent.height;
        leftBottom.x = leftBottomX * parent.width;
        leftBottom.y = leftBottomY * parent.height;
    }

    MovablePoint {
        id: leftTop
        size: pointSize
        borderColor: parent.color
        onXChanged: {
            if (x < 0) x = 0;
            if (x > parent.width) x = parent.width;
            leftTopX = x / parent.width;
        }
        onYChanged: {
            if (y < 0) y = 0;
            if (y > parent.height) y = parent.height;
            leftTopY = y / parent.height;
        }

        Text {
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: pointSize / 2
            anchors.topMargin: pointSize / 2
            font.bold: true
            color: parent.borderColor
            text: leftTopX.toFixed(3) + ', ' + leftTopY.toFixed(3)
        }
    }

    MovablePoint {
        id: rightTop
        size: pointSize
        borderColor: parent.color
        onXChanged: {
            if (x < 0) x = 0;
            if (x > parent.width) x = parent.width;
            rightTopX = x / parent.width;
        }
        onYChanged: {
            if (y < 0) y = 0;
            if (y > parent.height) y = parent.height;
            rightTopY = y / parent.height;
        }

        Text {
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.rightMargin: pointSize / 2
            anchors.topMargin: pointSize / 2
            font.bold: true
            color: parent.borderColor
            text: rightTopX.toFixed(3) + ', ' + rightTopY.toFixed(3)
        }
    }

    MovablePoint {
        id: rightBottom
        size: pointSize
        borderColor: parent.color
        onXChanged: {
            if (x < 0) x = 0;
            if (x > parent.width) x = parent.width;
            rightBottomX = x / parent.width;
        }
        onYChanged: {
            if (y < 0) y = 0;
            if (y > parent.height) y = parent.height;
            rightBottomY = y / parent.height;
        }

        Text {
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.rightMargin: pointSize / 2
            anchors.bottomMargin: pointSize / 2
            font.bold: true
            color: parent.borderColor
            text: rightBottomX.toFixed(3) + ', ' + rightBottomY.toFixed(3)
        }
    }

    MovablePoint {
        id: leftBottom
        size: pointSize
        borderColor: parent.color
        onXChanged: {
            if (x < 0) x = 0;
            if (x > parent.width) x = parent.width;
            leftBottomX = x / parent.width;
        }
        onYChanged: {
            if (y < 0) y = 0;
            if (y > parent.height) y = parent.height;
            leftBottomY = y / parent.height;
        }

        Text {
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.leftMargin: pointSize / 2
            anchors.bottomMargin: pointSize / 2
            font.bold: true
            color: parent.borderColor
            text: leftBottomX.toFixed(3) + ', ' + leftBottomY.toFixed(3)
        }
    }

    Line {
        x1: leftTop.x
        y1: leftTop.y
        x2: rightTop.x
        y2: rightTop.y
        color: parent.color
    }

    Line {
        x1: rightTop.x
        y1: rightTop.y
        x2: rightBottom.x
        y2: rightBottom.y
        color: parent.color
    }

    Line {
        x1: rightBottom.x
        y1: rightBottom.y
        x2: leftBottom.x
        y2: leftBottom.y
        color: parent.color
    }

    Line {
        x1: leftBottom.x
        y1: leftBottom.y
        x2: leftTop.x
        y2: leftTop.y
        color: parent.color
    }
}
