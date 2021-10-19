import QtQuick 2.15
import QtQuick.Controls 2.15

import spica.frontend 1.0

Item {
    id: winDetection
    anchors.fill: parent

    Image {
        id: image
        x: frameBorderWidth / 2
        y: frameBorderWidth / 2
        width: 400
        height: 300
        fillMode: Image.Stretch
        source: ""
    }

    Button {
        id: buttonRefresh
        anchors.horizontalCenter: image.horizontalCenter
        anchors.top: image.bottom
        text: "Refresh"

        onClicked: {
            console.log(Frontend.getRecentImages(10))
            image.source = Frontend.getRecentImages()
        }
    }
}
