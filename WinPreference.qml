import QtQuick 2.12
import QtQuick.Controls 2.12
import Qt.labs.qmlmodels 1.0

import spica.devplc 1.0

Item {
    id: winPreference
    anchors.fill: parent

    DevPLC {
    }

    TableView {
        anchors.fill: parent
        columnSpacing: 1
        rowSpacing: 1
        clip: true

        model: TableModel {
            TableModelColumn { display: "Station" }
            TableModelColumn { display: "1" }
            TableModelColumn { display: "2" }
            TableModelColumn { display: "3" }
            TableModelColumn { display: "4" }
            TableModelColumn { display: "5" }
            TableModelColumn { display: "6" }
            TableModelColumn { display: "7" }
            TableModelColumn { display: "8" }
            TableModelColumn { display: "9" }
            TableModelColumn { display: "10" }
            TableModelColumn { display: "11" }
            TableModelColumn { display: "12" }
        }

        delegate: TextInput {
            text: model.display
            padding: 12
            selectByMouse: true
        }
    }
}
