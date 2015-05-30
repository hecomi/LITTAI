import QtQuick 2.0

Item {
    property real size: 10
    property color borderColor: "#ff0000"

    Rectangle {
        id: circle
        color: "#00000000"
        border.color: borderColor
        smooth: true

        x: -size / 2
        y: -size / 2
        width: size
        height: size
        radius: size / 2

        MouseArea {
            id: dragMouseArea
            anchors.fill: parent
            drag.target: parent.parent
            drag.axis: Drag.XAndYAxis
            cursorShape: Qt.PointingHandCursor
            onPressed: {
                parent.color = borderColor;
            }
            onReleased: {
                parent.color = "#00000000";
            }
        }
    }
}
