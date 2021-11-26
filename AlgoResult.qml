import QtQuick 2.15
import QtQuick.Extras 1.4

import spica.frontend 1.0

Item {
    id: algoResult
    readonly property string type : "AlgoResult"
    property int number : -1
    property int pictureWidth : defaultPictureWidth
    property int pictureHeight : defaultPictureHeight
    property alias imageSource : image.source
    implicitHeight: pictureHeight * 1.2
    implicitWidth: pictureWidth * 1.2

    Rectangle {
        id: rect
        anchors.fill: parent

        Image {
            id: image
            x: 0
            y: 0
            width: pictureWidth
            height: pictureHeight
            fillMode: Image.Stretch
            source: ""
        }
    }
}
