// http://developer.nokia.com/community/wiki/Drawing_lines_in_QML

import QtQuick 2.0

Rectangle {
    id: l
    property alias x1: l.x
    property alias y1: l.y

    property real x2: l.x
    property real y2: l.y

    property color hoge: 'black'

    color: hoge
    height: 2
    smooth: true;

    transformOrigin: Item.TopLeft;

    width: getWidth(x1,y1,x2,y2);
    rotation: getSlope(x1,y1,x2,y2);

    function getWidth(sx1,sy1,sx2,sy2) {
        return Math.sqrt(Math.pow((sx2 - sx1), 2) + Math.pow((sy2 - sy1), 2));
    }

    function getSlope(sx1,sy1,sx2,sy2) {
        var a = sy2 - sy1;
        var b = sx2 - sx1;
        return Math.atan2(a, b) * 180 / Math.PI;
    }
}
