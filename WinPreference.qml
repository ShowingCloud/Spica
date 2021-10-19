import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt.labs.qmlmodels 1.0

import spica.devplc 1.0

Item {
    id: winPreference
    anchors.fill: parent

    DevPLC {
    }

    DevPLCServer {
    }

    Rectangle {
        width: (parent.width - frameBorderWidth) / 2
        height: parent.height - frameBorderWidth
        x: frameBorderWidth / 2
        y: frameBorderWidth / 2
        LabeledFrame {
            label: qsTr("Camera Preferences")
        }
    }

    Rectangle {
        width: (parent.width - frameBorderWidth) / 2
        height: parent.height - frameBorderWidth
        x: parent.width / 2
        y: frameBorderWidth / 2
        LabeledFrame {
            label: qsTr("PLC Preferences")
        }
    }
}
