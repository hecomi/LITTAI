import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

ColumnLayout {
    property double min   : 0.0
    property double max   : 100.0
    property double value :
        (isInteger) ?
            Math.floor(min + slider.value * (max - min)) :
            (min + slider.value * (max - min))
    property string label : 'No Name'
    property double defaultValue : 0.0
    property alias  sliderWidth  : slider.width_
    property alias  textWidth    : input.width_
    property int    fixedLength  : 2
    property bool   isInteger    : false

    Text {
        text: label
        font.pixelSize: 14
    }

    RowLayout {
        Slider {
            id: slider
            property int width_: 100
            Layout.preferredWidth: width_
            value: (defaultValue - min) / (max - min)
        }
        TextField {
            id: input
            property int width_: 50
            Layout.preferredWidth: width_
            text: (isInteger) ?
                value.toFixed(0) :
                value.toFixed(fixedLength)
            onAccepted: setValue(text);
        }
    }

    function setValue(newValue) {
        if (!newValue) return;

        if (newValue > max) {
            slider.value = 1.0;
        } else if (newValue < min) {
            slider.value = 0.0;
        } else {
            slider.value = (newValue - min) / (max - min);
        }
    }
}
