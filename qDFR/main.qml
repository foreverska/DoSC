import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.3

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("qDFR")

    TextField {
        id: deviceConnField
        x: 119
        y: 34
        text: qsTr("")
    }

    Text {
        id: element
        x: 39
        y: 46
        text: qsTr("CAN Device:")
        font.pixelSize: 12
    }

    Button {
        id: deviceConnBttn
        x: 325
        y: 34
        text: qsTr("Connect")
        onClicked: {
            dfr.canConnect(deviceConnField.text);
        }
    }

    ListView {
        id: dtcList
        x: 39
        y: 112
        width: 526
        height: 299
        delegate: Item {
            x: 5
            width: 80
            height: 40
            Row {
                id: row1
                spacing: 10
                Text {
                    text: modelData
                    anchors.verticalCenter: parent.verticalCenter
                    font.bold: true
                }
            }
        }
        model: dfr.curDtcs
    }

    Button {
        id: getDTCBtn
        x: 39
        y: 417
        text: qsTr("Get DTCs")
        onClicked: {
            dfr.getDTCs();
        }
    }

    Button {
        id: clearDTCBtn
        x: 145
        y: 417
        text: qsTr("Clear DTCs")
        onClicked: {
            dfr.clearDTCs();
        }
    }
}
