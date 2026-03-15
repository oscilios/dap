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
            x: dial.width / 2 - width / 2
            y: dial.height / 2 - height / 2
            width: 58
            height: 58

            // Outer shadow ring
            Rectangle {
                anchors.fill: parent
                radius: width / 2
                color: "transparent"
                border.color: "#1a1a1a"
                border.width: 2
            }

            // Knurled edge (tick marks around the perimeter)
            Repeater {
                model: 32
                Rectangle {
                    readonly property real a: index * (2 * Math.PI / 32)
                    readonly property real r: parent.width / 2
                    x: parent.width / 2 + (r - 1.5) * Math.cos(a) - 0.5
                    y: parent.height / 2 + (r - 1.5) * Math.sin(a) - 0.5
                    width: 1
                    height: 1
                    radius: 0.5
                    color: "#555555"
                }
            }

            // Main knob body — layered for metallic look
            Rectangle {
                anchors.centerIn: parent
                width: parent.width - 6
                height: parent.height - 6
                radius: width / 2
                color: "#353535"
                border.color: "#444444"
                border.width: 0.5
            }

            // Highlight (upper-left light reflection)
            Rectangle {
                anchors.centerIn: parent
                anchors.horizontalCenterOffset: -3
                anchors.verticalCenterOffset: -3
                width: parent.width - 16
                height: parent.height - 16
                radius: width / 2
                color: "#404040"
                opacity: 0.6
            }

            // Inner cap (slightly raised center)
            Rectangle {
                anchors.centerIn: parent
                width: parent.width - 22
                height: parent.height - 22
                radius: width / 2
                color: "#2e2e2e"
                border.color: "#3d3d3d"
                border.width: 0.5
            }
        }

        handle: Item {
            id: handleItem
            readonly property real angleRad: dial.angle * Math.PI / 180
            readonly property real orbitRadius: 0
            readonly property real centerX: dial.background.x + dial.background.width / 2
            readonly property real centerY: dial.background.y + dial.background.height / 2

            x: centerX - width / 2
            y: centerY - height / 2
            width: dial.background.width - 6
            height: dial.background.height - 6

            // White indicator line from center toward edge
            Rectangle {
                readonly property real lineLen: parent.width / 2 - 4
                readonly property real angleRad: handleItem.angleRad
                x: parent.width / 2 + (lineLen * 0.35) * Math.sin(angleRad) - width / 2
                y: parent.height / 2 - (lineLen * 0.35) * Math.cos(angleRad) - height / 2
                width: 2.5
                height: lineLen * 0.65
                radius: 1.25
                color: "#e0e0e0"
                antialiasing: true
                transform: Rotation {
                    origin.x: 1.25
                    origin.y: 0
                    angle: handleItem.angleRad * 180 / Math.PI
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
