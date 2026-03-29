import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Sub37

GroupBox {
    title: "PITCH ENVELOPE"
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

    RowLayout {
        spacing: 8
        RotarySlider {
            label: "Amt"
            from: -2400; to: 2400; value: SynthBridge.pitchEnvAmount
            stepSize: 10; decimals: 0
            onValueChanged: SynthBridge.pitchEnvAmount = value
        }
        RotarySlider {
            label: "A"
            from: 0.001; to: 5; value: SynthBridge.pitchEnvAttack
            stepSize: 0.001; decimals: 3
            onValueChanged: SynthBridge.pitchEnvAttack = value
        }
        RotarySlider {
            label: "D"
            from: 0.01; to: 10; value: SynthBridge.pitchEnvDecay
            stepSize: 0.01; decimals: 2
            onValueChanged: SynthBridge.pitchEnvDecay = value
        }
        RotarySlider {
            label: "S"
            from: 0; to: 1; value: SynthBridge.pitchEnvSustain
            onValueChanged: SynthBridge.pitchEnvSustain = value
        }
        RotarySlider {
            label: "R"
            from: 0.01; to: 10; value: SynthBridge.pitchEnvRelease
            stepSize: 0.01; decimals: 2
            onValueChanged: SynthBridge.pitchEnvRelease = value
        }
    }
}
