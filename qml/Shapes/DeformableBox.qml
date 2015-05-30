import QtQuick 2.0

Item {
    property real leftTopX: 10
    property real leftTopY: 10
    property real leftBottomX: 10
    property real leftBottomY: 100
    property real rightTopX: 200
    property real rightTopY: 10
    property real rightBottomX: 200
    property real rightBottomY: 100

    property real normalizeWidth: 1000;
    property real normalizeHeight: 1000;

    property real pointSize: 10
    property color color: "#ff0000"

    property variant points: [
        [leftTopX / normalizeWidth,     leftTopY / normalizeHeight],
        [rightTopX / normalizeWidth,    rightTopY / normalizeHeight],
        [rightBottomX / normalizeWidth, rightBottomY / normalizeHeight],
        [leftBottomX / normalizeWidth,  leftBottomY / normalizeHeight]
    ]

    MovablePoint {
        id: leftTop
        x: leftTopX
        y: leftTopY
        size: pointSize
        borderColor: parent.color
        onXChanged: {
            if (x < 0) x = 0;
            if (normalizeWidth > 0 && x > normalizeWidth) x = normalizeWidth;
            leftTopX = x;
        }
        onYChanged: {
            if (y < 0) y = 0;
            if (normalizeHeight > 0 && y > normalizeHeight) y = normalizeHeight;
            leftTopY = y;
        }

        Text {
            anchors.right: parent.left
            anchors.bottom: parent.top
            anchors.rightMargin: pointSize / 2
            anchors.bottomMargin: pointSize / 2
            font.bold: true
            color: parent.borderColor
            text: (parent.x / normalizeWidth).toFixed(3) + ', ' + (parent.y / normalizeHeight).toFixed(3)
        }
    }

    MovablePoint {
        id: rightTop
        x: rightTopX
        y: rightTopY
        size: pointSize
        borderColor: parent.color
        onXChanged: {
            if (x < 0) x = 0;
            if (normalizeWidth > 0 && x > normalizeWidth) x = normalizeWidth;
            rightTopX = x;
        }
        onYChanged: {
            if (y < 0) y = 0;
            if (normalizeHeight > 0 && y > normalizeHeight) y = normalizeHeight;
            rightTopY = y;
        }

        Text {
            anchors.left: parent.right
            anchors.bottom: parent.top
            anchors.rightMargin: pointSize / 2
            anchors.bottomMargin: pointSize / 2
            font.bold: true
            color: parent.borderColor
            text: (parent.x / normalizeWidth).toFixed(3) + ', ' + (parent.y / normalizeHeight).toFixed(3)
        }
    }

    MovablePoint {
        id: rightBottom
        x: rightBottomX
        y: rightBottomY
        size: pointSize
        borderColor: parent.color
        onXChanged: {
            if (x < 0) x = 0;
            if (normalizeWidth > 0 && x > normalizeWidth) x = normalizeWidth;
            rightBottomX = x;
        }
        onYChanged: {
            if (y < 0) y = 0;
            if (normalizeHeight > 0 && y > normalizeHeight) y = normalizeHeight;
            rightBottomY = y;
        }

        Text {
            anchors.left: parent.right
            anchors.top: parent.bottom
            anchors.leftMargin: pointSize / 2
            anchors.topMargin: pointSize / 2
            font.bold: true
            color: parent.borderColor
            text: (parent.x / normalizeWidth).toFixed(3) + ', ' + (parent.y / normalizeHeight).toFixed(3)
        }
    }

    MovablePoint {
        id: leftBottom
        x: leftBottomX
        y: leftBottomY
        size: pointSize
        borderColor: parent.color
        onXChanged: {
            if (x < 0) x = 0;
            if (normalizeWidth > 0 && x > normalizeWidth) x = normalizeWidth;
            leftBottomX = x;
        }
        onYChanged: {
            if (y < 0) y = 0;
            if (normalizeHeight > 0 && y > normalizeHeight) y = normalizeHeight;
            leftBottomY = y;
        }

        Text {
            anchors.right: parent.left
            anchors.top: parent.bottom
            anchors.rightMargin: pointSize / 2
            anchors.topMargin: pointSize / 2
            font.bold: true
            color: parent.borderColor
            text: (parent.x / normalizeWidth).toFixed(3) + ', ' + (parent.y / normalizeHeight).toFixed(3)
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
