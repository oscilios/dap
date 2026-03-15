import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Sub37

ScrollView {
    contentWidth: availableWidth

    ColumnLayout {
        width: parent.width
        spacing: 16

        // Amp Envelope
        GroupBox {
            title: "Amplifier Envelope"
            Layout.fillWidth: true
            RowLayout {
                spacing: 12
                RotarySlider {
                    label: "Attack"
                    from: 0.001; to: 5; value: SynthBridge.ampAttack
                    stepSize: 0.001; decimals: 3
                    onValueChanged: SynthBridge.ampAttack = value
                }
                RotarySlider {
                    label: "Decay"
                    from: 0.01; to: 10; value: SynthBridge.ampDecay
                    stepSize: 0.01; decimals: 2
                    onValueChanged: SynthBridge.ampDecay = value
                }
                RotarySlider {
                    label: "Sustain"
                    from: 0; to: 1; value: SynthBridge.ampSustain
                    onValueChanged: SynthBridge.ampSustain = value
                }
                RotarySlider {
                    label: "Release"
                    from: 0.01; to: 10; value: SynthBridge.ampRelease
                    stepSize: 0.01; decimals: 2
                    onValueChanged: SynthBridge.ampRelease = value
                }
            }
        }
    }
}
