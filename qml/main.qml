import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import Littai 1.0
import 'views'
import 'shapes'
import 'common'
import 'styles'

ApplicationWindow {
    id: window
    title: 'LITTAI'
    visible: true
    width: 1600
    height: 900

    property var homographyImage: null
    property var diffImage: null
    property alias osc : osc

    Storage {
        id: storage
        name: 'LITTAI'
        description: 'Interaction recognizer for LITTAI project.'
    }

    Osc {
        id: osc
        ip: storage.get('osc.ip') || '127.0.0.1'
        port: storage.get('osc.port') || 4567
    }

    TabView {
        id: tabView
        frameVisible: true
        anchors.fill: parent
        anchors.margins: 12
        clip: true

        Tab {
            id: homographyTab
            title: "Homography"
            anchors.margins: 4
            anchors.topMargin: 24

            HomographyView {
                id: homographyView
                anchors.fill: parent
            }
        }

        Tab {
            id: diffTab
            title: "Diff"
            anchors.margins: 4
            anchors.topMargin: 12
            property var diffView: item ? item.diffView : null

            DiffView {
                property var diffView: this
                anchors.fill: parent
            }
        }

        Tab {
            id: landoltTrackerTab
            title: "Landolt"
            anchors.margins: 4
            anchors.topMargin: 12

            LandoltTrackView {
                id: landoltTrackView
                anchors.fill: parent
            }
        }

        Tab {
            id: markerTrackerTab
            title: "Marker"
            anchors.margins: 4
            anchors.topMargin: 12

            MarkerTrackView {
                id: markerTrackView
                anchors.fill: parent
            }
        }

        Tab {
            id: settingTab
            title: "Settings"
            anchors.margins: 24

            ColumnLayout {

                GroupBox {
                    title: "OSC"
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignTop

                    ColumnLayout {

                        RowLayout {
                            Text {
                                text: "IP"
                                font.pixelSize: 14
                            }

                            TextField {
                                id: oscIpText
                                property int width_: 120
                                text: osc.ip
                                onAccepted: {
                                    storage.set('osc.ip', text)
                                    osc.ip = text;
                                }
                            }
                        }

                        RowLayout {
                            Text {
                                text: "Port"
                                font.pixelSize: 14
                            }

                            TextField {
                                id: oscIpPort
                                property int width_: 120
                                text: osc.port
                                onAccepted: {
                                    var port = parseInt(text);
                                    storage.set('osc.port', port)
                                    osc.port = port;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Item {
        anchors.fill: parent
        focus: true

        Keys.onPressed: {
            switch (event.key) {
                case Qt.Key_Right:
                    var index = (tabView.currentIndex + 1) % tabView.count;
                    tabView.currentIndex = index;
                    break;
                case Qt.Key_Left:
                    var index = tabView.currentIndex - 1;
                    if (index < 1) index = tabView.count - 1;
                    tabView.currentIndex = index;
                    break;
                case Qt.Key_C:
                    if (diffTab.diffView) {
                        diffTab.diffView.setBaseImage();
                    }
                    break;
            }
        }
    }
}
