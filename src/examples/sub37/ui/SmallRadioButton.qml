import QtQuick
import QtQuick.Controls

RadioButton {
    id: root
    padding: 0
    topInset: 0; bottomInset: 0
    implicitHeight: 18

    indicator: Rectangle {
        implicitWidth: 10
        implicitHeight: 10
        x: 0
        y: (root.height - height) / 2
        radius: 5
        color: "transparent"
        border.color: root.checked ? "#c0c0c0" : "#555555"
        border.width: 1

        Rectangle {
            anchors.centerIn: parent
            width: 5; height: 5; radius: 2.5
            color: "#e0e0e0"
            visible: root.checked
        }
    }
}
