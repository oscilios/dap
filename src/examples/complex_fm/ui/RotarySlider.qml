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
        color: "#cccccc"
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
        Layout.preferredWidth: 60
        Layout.preferredHeight: 60
        Layout.alignment: Qt.AlignHCenter
        inputMode: Dial.Circular

        onMoved: root.value = dial.value

        background: Rectangle {
            x: dial.width / 2 - width / 2
            y: dial.height / 2 - height / 2
            width: 56
            height: 56
            radius: 28
            color: "#333333"
            border.color: "#666666"
            border.width: 1
        }

        handle: Rectangle {
            id: handleItem
            readonly property real angleRad: dial.angle * Math.PI / 180
            readonly property real orbitRadius: dial.background.width / 2 - width / 2 - 4
            readonly property real centerX: dial.background.x + dial.background.width / 2
            readonly property real centerY: dial.background.y + dial.background.height / 2
            x: centerX + orbitRadius * Math.sin(angleRad) - width / 2
            y: centerY - orbitRadius * Math.cos(angleRad) - height / 2
            width: 8
            height: 8
            radius: 4
            color: "#00bcd4"
        }
    }

    Label {
        text: root.value.toFixed(root.decimals)
        font.pixelSize: 10
        color: "#aaaaaa"
        Layout.alignment: Qt.AlignHCenter
    }
}
