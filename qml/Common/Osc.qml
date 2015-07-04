import QtQuick 2.1
import Littai 1.0

Item {
    id: impl
    property alias ip: osc.ip
    property alias port: osc.port

    OscSender {
        id: osc
    }

    function send(address, msg) {
        osc.send(address, JSON.stringify(msg));
    }
}
