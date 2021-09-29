import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

Window {
    readonly property int heightWindow: 800
    readonly property int widthWindow: 1550
    readonly property int heightTabbar: 40
    readonly property int heightStack: heightWindow - heightTabbar
    readonly property int widthStack: widthWindow

    readonly property int defaultMarginAndTextWidthHeight: 20
    readonly property int frameBorderWidth: 20
    readonly property int defaultLabelFontSize: 20
    readonly property int rectBigFontSize: 15
    readonly property int rectSmallFontSize: 12 // 8

    visible: true
    width: 1550
    height: 800
    title: qsTr("Deep Learning Defect Detection")

    TabBar {
        id: bar
        width: parent.width
        TabButton {
            text: qsTr("Preference")
            font.pixelSize: defaultLabelFontSize
        }
        TabButton {
            text: qsTr("Camera State")
            font.pixelSize: defaultLabelFontSize
        }
        TabButton {
            text: qsTr("Real Time Detection")
            font.pixelSize: defaultLabelFontSize
        }
        TabButton {
            text: qsTr("History")
            font.pixelSize: defaultLabelFontSize
        }
        TabButton {
            text: qsTr("Statistics")
            font.pixelSize: defaultLabelFontSize
        }
        TabButton {
            text: qsTr("Data Tagging")
            font.pixelSize: defaultLabelFontSize
        }

    }

    StackLayout {
        anchors.top: bar.bottom
        anchors.bottom: parent.bottom
        width: parent.width
        currentIndex: bar.currentIndex

        Item {
            id: tabPreference
            LabeledFrame {
                label: qsTr("Preference")
                WinPreference{}
            }
        }
        Item {
            id: tabState
            LabeledFrame { label: qsTr("Camera State") }
        }
        Item {
            id: tabDetection
            LabeledFrame { label: qsTr("Real Time Detection") }
        }
        Item {
            id: tabHistory
            LabeledFrame { label: qsTr("History") }
        }
        Item {
            id: tabStatistics
            LabeledFrame { label: qsTr("Statistics") }
        }
        Item {
            id: tabTagging
            LabeledFrame { label: qsTr("Data Tagging") }
        }
    }
}
