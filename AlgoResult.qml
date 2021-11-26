import QtQuick 2.15
import QtQuick.Extras 1.4
import QtQuick.Shapes 1.15
import QtQml.Models 2.15

import spica.frontend 1.0

Item {
    id: algoResult
    readonly property string type : "AlgoResult"
    property int number : -1
    property int pictureWidth : defaultPictureWidth
    property int pictureHeight : defaultPictureHeight
    property alias imageSource : image.source
    property alias modelAreas : modelAreas
    property alias modelArea : modelArea
    implicitHeight: pictureHeight * 1.2
    implicitWidth: pictureWidth * 1.2

    signal reset()

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

        ListModel {
            id: modelAreas
            Component.onCompleted: algoResult.reset.connect(clear)
        }

        ListModel {
            id: modelArea
            Component.onCompleted: algoResult.reset.connect(clear)
        }

        Shape {
            id: shapeAreas
            anchors.fill: image
            Component.onCompleted: algoResult.reset.connect(function() {data = [empty]})
            ShapePath { id: empty }
        }

        Instantiator {
            model: modelAreas
            onObjectAdded: shapeAreas.data.push(object)
            ShapePath {
                fillColor: "transparent"
                strokeColor: "red"
                strokeWidth: 1
                strokeStyle: ShapePath.SolidLine
                startX: model.x * pictureWidth / 1200
                startY: model.y * pictureHeight / 800
            }
        }

        Instantiator {
            model: modelArea
            onObjectAdded: shapeAreas.data[shapeAreas.data.length - 1].pathElements.push(object)
            PathLine {
                x: model.x * pictureWidth / 1200
                y: model.y * pictureHeight / 800
            }
        }
    }
}
