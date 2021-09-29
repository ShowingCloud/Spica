import QtQuick 2.12
import QtQuick.Controls 2.12
import Qt.labs.qmlmodels 1.0

import spica.devplc 1.0

Item {
    id: winPreference
    anchors.fill: parent
    anchors.margins: frameBorderWidth

    DevPLC {
    }

    DevPLCServer {
    }

    Rectangle {
        width: (parent.width - 2 * frameBorderWidth - defaultMarginAndTextWidthHeight) / 2
        height: parent.height - 2 * frameBorderWidth
        x: frameBorderWidth
        y: frameBorderWidth
        LabeledFrame {
            label: qsTr("Camera Preferences")
        }
    }

    Rectangle {
        width: (parent.width - 2 * frameBorderWidth - defaultMarginAndTextWidthHeight) / 2
        height: parent.height - 2 * frameBorderWidth
        x: (parent.width + defaultMarginAndTextWidthHeight) / 2
        y: frameBorderWidth
        LabeledFrame {
            label: qsTr("PLC Preferences")
        }
    }
}
