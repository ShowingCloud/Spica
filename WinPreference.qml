import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
    id: winPreference
    anchors.fill: parent

    TextArea {
        id: txt
        anchors.fill: parent
        wrapMode: TextEdit.Wrap
        focus: false
        readOnly: true
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 30
        text: "text"

        Component.onCompleted: console.log(parent.implicitHeight)
    }
}
