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

    readonly property int defaultMarginAndTextWidthHeight: 30
    readonly property int defaultLabelFontSize: 20
    readonly property int rectBigFontSize: 15
    readonly property int rectSmallFontSize: 12 // 8

    visible: true
    width: 1550
    height: 800
    title: qsTr("Deep Learning Defect Detection")

    Item {
        component LabeledFrame : Rectangle {
            property string title

            anchors.fill: parent
            anchors.margins: 20
            radius: 5
            border.width: 1

            Label {
                x: 15
                y: -10
                text: title
                font.pixelSize: defaultLabelFontSize
                background: Rectangle {
                    anchors.fill: parent
                    anchors.margins: -5
                    color: "white"
                }
            }
        }
    }

    TabBar {
        id: bar
        //height: 40
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

            LabeledFrame { title: qsTr("Preference") }
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
