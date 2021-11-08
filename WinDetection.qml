import QtQuick 2.15
import QtQuick.Controls 2.15

import spica.frontend 1.0

Item {
    id: winDetection
    anchors.fill: parent

    Image {
        id: image1
        x: frameBorderWidth / 2
        y: frameBorderWidth / 2
        width: 400
        height: 300
        fillMode: Image.Stretch
        source: ""
    }

    Image {
        id: image2
        anchors.left: image1.right
        anchors.leftMargin: defaultMarginAndTextWidthHeight
        anchors.top: image1.top
        width: 400
        height: 300
        fillMode: Image.Stretch
        source: ""
    }

    Image {
        id: image3
        anchors.left: image2.right
        anchors.leftMargin: defaultMarginAndTextWidthHeight
        anchors.top: image1.top
        width: 400
        height: 300
        fillMode: Image.Stretch
        source: ""
    }

    Image {
        id: image4
        anchors.left: image1.left
        anchors.top: image1.bottom
        anchors.topMargin: defaultMarginAndTextWidthHeight
        width: 400
        height: 300
        fillMode: Image.Stretch
        source: ""
    }

    Image {
        id: image5
        anchors.left: image2.left
        anchors.top: image1.bottom
        anchors.topMargin: defaultMarginAndTextWidthHeight
        width: 400
        height: 300
        fillMode: Image.Stretch
        source: ""
    }

    Image {
        id: image6
        anchors.left: image3.left
        anchors.top: image1.bottom
        anchors.topMargin: defaultMarginAndTextWidthHeight
        width: 400
        height: 300
        fillMode: Image.Stretch
        source: ""
    }

    Button {
        id: buttonRefresh
        anchors.horizontalCenter: image5.horizontalCenter
        anchors.top: image5.bottom
        anchors.topMargin: defaultMarginAndTextWidthHeight
        text: "Refresh"

        onClicked: {
            var imageList = Frontend.getRecentImages(10)
            image1.source = imageList[0]
            image2.source = imageList[1]
            image3.source = imageList[2]
            image4.source = imageList[3]
            image5.source = imageList[4]
            image6.source = imageList[5]
        }
    }
}
