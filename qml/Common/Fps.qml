import QtQuick 2.1

Text {
    property var fpsArray: new Array(0)
    property real previousTime: 0.0
    property int sampleNum: 30
    font.pixelSize: 24
    font.bold: true
    color: '#aaffffff'

    function update() {
        var currentTime = +new Date();
        if (currentTime === previousTime) return;

        var fps = 1000.0 / (currentTime - previousTime);
        fpsArray.push(fps);
        if (fpsArray.length > sampleNum) fpsArray.splice(0, 1);

        text = Math.floor(fpsArray.reduce(function(a, b) {
            return a + b;
        }) / sampleNum + 0.5).toFixed(0);

        previousTime = currentTime;
    }
}
