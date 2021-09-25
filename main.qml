import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

Window {
    visible: true
    width: 1550
    height: 800
    title: qsTr("Deep Learning Defect Detection")

    TabBar {
        id: bar
        width: parent.width
        TabButton {
            text: qsTr("Preference")
        }
        TabButton {
            text: qsTr("Camera State")
        }
        TabButton {
            text: qsTr("Real Time Detection")
        }
        TabButton {
            text: qsTr("History")
        }
        TabButton {
            text: qsTr("Statistics")
        }
        TabButton {
            text: qsTr("Data Tagging")
        }
    }

    StackLayout {
        anchors.top: bar.bottom
        anchors.bottom: parent.bottom
        width: parent.width
        currentIndex: bar.currentIndex

        Item {
            id: tabPreference
            WinPreference{}
        }
        Item {
            id: tabState
        }
        Item {
            id: tabDetection
        }
        Item {
            id: tabHistory
        }
        Item {
            id: tabStatistics
        }
        Item {
            id: tabTagging
        }
    }
}
