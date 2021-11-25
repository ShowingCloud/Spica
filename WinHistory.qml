import QtQuick 2.15
import QtQuick.Controls 2.15

import spica.frontend 1.0

Item {
    id: winHistory
    anchors.fill: parent

    property int defaultPictureWidth: 200
    property int defaultPictureHeight: 150

    HorizontalHeaderView {
        id: headerview
        x: frameBorderWidth / 2 + defaultMarginAndTextWidthHeight
        y: frameBorderWidth / 2 + defaultMarginAndTextWidthHeight
        syncView: tableview
    }

    Component.onCompleted: ProductRecordModel.fillData()

    TableView {
        id: tableview
        width: 50+120+8*20
        height: winHistory.height - frameBorderWidth - headerview.height - 2 * defaultMarginAndTextWidthHeight
        anchors.left: headerview.left
        anchors.top: headerview.bottom
        clip: true

        model: ProductRecordModel

        delegate: Rectangle {
            implicitHeight: 30
            implicitWidth: sizehint
            border.width: 1

            Text {
                text: display
                anchors.fill: parent
                anchors.margins: 10
                font.pixelSize: 8
                verticalAlignment: Qt.AlignVCenter
                horizontalAlignment: textalignment
                color: foreground
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    var images = ProductRecordModel.getImages(row)
                    image1.imageSource = images[0]
                    image2.imageSource = images[1]
                    image3.imageSource = images[2]
                    image4.imageSource = images[3]
                    image5.imageSource = images[4]
                    image6.imageSource = images[5]
                    image7.imageSource = images[6]
                    image8.imageSource = images[7]
                }
            }
        }

        ScrollIndicator.vertical: ScrollIndicator {}
    }

    Timer {
        id: timer
        interval: 1000
        running: true
        repeat: true

        onTriggered: ProductRecordModel.fillData()
    }

    AlgoResult {
        id: image1
        anchors.left: headerview.right
        anchors.leftMargin: defaultMarginAndTextWidthHeight
        anchors.top: headerview.top
    }

    AlgoResult {
        id: image2
        anchors.left: image1.right
        anchors.leftMargin: defaultMarginAndTextWidthHeight
        anchors.top: image1.top
    }

    AlgoResult {
        id: image3
        anchors.left: image2.right
        anchors.leftMargin: defaultMarginAndTextWidthHeight
        anchors.top: image1.top
    }

    AlgoResult {
        id: image4
        anchors.left: image3.right
        anchors.leftMargin: defaultMarginAndTextWidthHeight
        anchors.top: image1.top
    }

    AlgoResult {
        id: image5
        anchors.left: image1.left
        anchors.top: image1.bottom
        anchors.topMargin: defaultMarginAndTextWidthHeight
    }

    AlgoResult {
        id: image6
        anchors.left: image2.left
        anchors.top: image1.bottom
        anchors.topMargin: defaultMarginAndTextWidthHeight
    }

    AlgoResult {
        id: image7
        anchors.left: image3.left
        anchors.top: image1.bottom
        anchors.topMargin: defaultMarginAndTextWidthHeight
    }

    AlgoResult {
        id: image8
        anchors.left: image4.left
        anchors.top: image1.bottom
        anchors.topMargin: defaultMarginAndTextWidthHeight
    }
}
