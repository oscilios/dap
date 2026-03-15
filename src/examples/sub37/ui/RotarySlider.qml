import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    id: root

    property string label: ""
    property real from: 0.0
    property real to: 1.0
    property real value: 0.0
    property int decimals: 2
    property real stepSize: 0.01

    spacing: 2
    implicitWidth: 80

    Label {
        text: root.label
        font.pixelSize: 10
        font.family: "Helvetica Neue"
        font.weight: Font.Medium
        color: "#b0b0b0"
        Layout.alignment: Qt.AlignHCenter
        elide: Text.ElideRight
        Layout.maximumWidth: parent.width
    }

    Dial {
        id: dial
        from: root.from
        to: root.to
        value: root.value
        stepSize: root.stepSize
        Layout.preferredWidth: 64
        Layout.preferredHeight: 64
        Layout.alignment: Qt.AlignHCenter
        inputMode: Dial.Circular

        onMoved: root.value = dial.value

        background: Item {
            id: bg
            x: dial.width / 2 - width / 2
            y: dial.height / 2 - height / 2
            width: 60
            height: 60

            // Outer track arc with gap at bottom
            Canvas {
                anchors.fill: parent
                onPaint: {
                    var ctx = getContext("2d");
                    ctx.reset();
                    var cx = width / 2, cy = height / 2;
                    var r = width / 2 - 2;
                    ctx.strokeStyle = "#555555";
                    ctx.lineWidth = 3;
                    ctx.lineCap = "round";
                    ctx.beginPath();
                    // Arc from 130° to 50° (clockwise through top), leaving 80° gap at bottom
                    ctx.arc(cx, cy, r, 130 * Math.PI / 180, 50 * Math.PI / 180, false);
                    ctx.stroke();
                }
            }

            // Knob body
            Rectangle {
                id: knobBody
                anchors.centerIn: parent
                width: parent.width - 8
                height: parent.height - 8
                radius: width / 2
                color: "#1a1a1a"
                border.color: "#2a2a2a"
                border.width: 1
            }

            // Inner cap ring
            Rectangle {
                id: innerRing
                anchors.centerIn: parent
                width: parent.width * 0.42
                height: width
                radius: width / 2
                color: "transparent"
                border.color: "#c0c0c0"
                border.width: 2
            }
        }

        handle: Item {
            id: handleItem
            x: bg.x + bg.width / 2 - width / 2
            y: bg.y + bg.height / 2 - height / 2
            width: knobBody.width
            height: knobBody.height

            // Indicator line from near-edge to center
            Rectangle {
                id: indicator
                x: handleItem.width / 2 - width / 2
                y: 3
                width: 3
                height: handleItem.height / 2 - 3
                radius: 1.5
                color: "#c0c0c0"
                antialiasing: true
                transform: Rotation {
                    origin.x: indicator.width / 2
                    origin.y: handleItem.height / 2 - indicator.y
                    angle: dial.angle
                }
            }
        }
    }

    Label {
        text: root.value.toFixed(root.decimals)
        font.pixelSize: 9
        font.family: "Helvetica Neue"
        color: "#888888"
        Layout.alignment: Qt.AlignHCenter
    }
}
