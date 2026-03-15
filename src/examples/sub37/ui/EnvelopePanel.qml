import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Sub37

GroupBox {
    title: "AMP ENVELOPE"
    label: Label {
        text: parent.title
        color: "#d4a017"
        font.pixelSize: 10
        font.family: "Helvetica Neue"
        font.letterSpacing: 2
        font.weight: Font.Medium
        leftPadding: 8
    }
    background: Rectangle {
        color: "transparent"
        border.color: "#333333"
        border.width: 1
        radius: 3
        y: parent.topPadding - parent.bottomPadding
        height: parent.height - parent.topPadding + parent.bottomPadding
    }

    ColumnLayout {
        spacing: 2
        Label {
            text: "ADSR"
            color: "#b0b0b0"
            font.pixelSize: 10
            font.weight: Font.Medium
            Layout.alignment: Qt.AlignHCenter
        }
        RowLayout {
            spacing: 8
            RotarySlider {
                label: "A"
                from: 0.001; to: 5; value: SynthBridge.ampAttack
                stepSize: 0.001; decimals: 3
                onValueChanged: SynthBridge.ampAttack = value
            }
            RotarySlider {
                label: "D"
                from: 0.01; to: 10; value: SynthBridge.ampDecay
                stepSize: 0.01; decimals: 2
                onValueChanged: SynthBridge.ampDecay = value
            }
            RotarySlider {
                label: "S"
                from: 0; to: 1; value: SynthBridge.ampSustain
                onValueChanged: SynthBridge.ampSustain = value
            }
            RotarySlider {
                label: "R"
                from: 0.01; to: 10; value: SynthBridge.ampRelease
                stepSize: 0.01; decimals: 2
                onValueChanged: SynthBridge.ampRelease = value
            }
        }
    }
}
