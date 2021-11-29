import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt.labs.qmlmodels 1.0

import spica.frontend 1.0

Item {
    id: winPreference
    anchors.fill: parent

    Rectangle {
        id: rectCamera
        width: (parent.width - frameBorderWidth) / 2
        height: parent.height - frameBorderWidth
        x: frameBorderWidth / 2
        y: frameBorderWidth / 2
        LabeledFrame {
            label: qsTr("Camera Preferences")
        }
    }

    Rectangle {
        id: rectPLC
        width: (parent.width - frameBorderWidth) / 2
        height: parent.height - frameBorderWidth
        x: parent.width / 2
        y: frameBorderWidth / 2
        LabeledFrame {
            label: qsTr("PLC Preferences")
        }
    }

    Button {
        id: buttonCamera
        anchors.horizontalCenter: rectCamera.horizontalCenter
        anchors.top: rectCamera.bottom
        text: "Refresh"

        onClicked: {
            var info = Frontend.getSerialInfo()
            for (var serial in info)
                for (var prop in info[serial])
                    console.log(serial, prop, info[serial][prop])

            info = Frontend.getCameraInfo()
            for (var camera in info)
                for (prop in info[camera])
                    console.log(camera, prop, info[camera][prop])
        }
    }
}
