import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
    property string label

    anchors.fill: parent
    anchors.margins: frameBorderWidth

    Rectangle {
        anchors.fill: parent
        radius: 5
        border.width: 1

        Label {
            x: 15
            y: -10
            text: label
            font.pixelSize: defaultLabelFontSize
            background: Rectangle {
                anchors.fill: parent
                anchors.margins: -5
                color: "white"
            }
        }
    }
}
