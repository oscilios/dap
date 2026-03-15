import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Sub37

ScrollView {
    contentWidth: availableWidth

    ColumnLayout {
        width: parent.width
        spacing: 16

        // Ladder Filter
        GroupBox {
            title: "Moog Ladder Filter"
            Layout.fillWidth: true
            RowLayout {
                spacing: 12
                RotarySlider {
                    label: "Cutoff"
                    from: 20; to: 20000; value: SynthBridge.filterCutoff
                    stepSize: 10; decimals: 0
                    onValueChanged: SynthBridge.filterCutoff = value
                }
                RotarySlider {
                    label: "Resonance"
                    from: 0; to: 1; value: SynthBridge.filterResonance
                    onValueChanged: SynthBridge.filterResonance = value
                }
                RotarySlider {
                    label: "Env Amount"
                    from: 0; to: 10000; value: SynthBridge.filterEnvAmount
                    stepSize: 10; decimals: 0
                    onValueChanged: SynthBridge.filterEnvAmount = value
                }
            }
        }

        // Filter Envelope
        GroupBox {
            title: "Filter Envelope"
            Layout.fillWidth: true
            RowLayout {
                spacing: 12
                RotarySlider {
                    label: "Attack"
                    from: 0.001; to: 5; value: SynthBridge.filterAttack
                    stepSize: 0.001; decimals: 3
                    onValueChanged: SynthBridge.filterAttack = value
                }
                RotarySlider {
                    label: "Decay"
                    from: 0.01; to: 10; value: SynthBridge.filterDecay
                    stepSize: 0.01; decimals: 2
                    onValueChanged: SynthBridge.filterDecay = value
                }
                RotarySlider {
                    label: "Sustain"
                    from: 0; to: 1; value: SynthBridge.filterSustain
                    onValueChanged: SynthBridge.filterSustain = value
                }
                RotarySlider {
                    label: "Release"
                    from: 0.01; to: 10; value: SynthBridge.filterRelease
                    stepSize: 0.01; decimals: 2
                    onValueChanged: SynthBridge.filterRelease = value
                }
            }
        }
    }
}
