import QtQuick 2.1
import QtQuick.Controls.Styles 1.0

TabViewStyle {
    tabOverlap   : 16
    frameOverlap : 4
    tabsMovable  : true

    frame: Rectangle {
        border.color: '#898989'
        gradient: Gradient {
            GradientStop { color: '#e5e5e5' ; position: 0 }
            GradientStop { color: '#e0e0e0' ; position: 1 }
        }
        Rectangle {
            anchors.fill: parent
            anchors.margins: 1
            border.color: 'white'
            color: 'transparent'
        }
    }

    tab: Item {
        property int totalOverlap : tabOverlap * (control.count - 1)
        implicitWidth  : Math.min ((styleData.availableWidth + totalOverlap)/control.count - 4, image.sourceSize.width)
        implicitHeight : image.sourceSize.height
        BorderImage {
            id           : image
            anchors.fill : parent
            source       : styleData.selected ? 'images/tab_selected.png' : 'images/tab.png'
            border.left  : 30
            smooth       : false
            border.right : 30
        }
        Text {
            text: styleData.title
            anchors.centerIn: parent
        }
    }

    leftCorner: Item { implicitWidth: 12 }
}
