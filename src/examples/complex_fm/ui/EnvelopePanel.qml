import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ComplexFM

ScrollView {
    contentWidth: availableWidth

    ColumnLayout {
        width: parent.width
        spacing: 16

        // Amplitude Envelope
        GroupBox {
            title: "Amplitude Envelope"
            Layout.fillWidth: true
            RowLayout {
                spacing: 12
                RotarySlider {
                    label: "Attack"
                    from: 0.001; to: 5; value: SynthBridge.ampAttack
                    stepSize: 0.01; decimals: 2
                    onValueChanged: SynthBridge.ampAttack = value
                }
                RotarySlider {
                    label: "Decay"
                    from: 0.001; to: 5; value: SynthBridge.ampDecay
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
                    from: 0.001; to: 5; value: SynthBridge.ampRelease
                    stepSize: 0.01; decimals: 2
                    onValueChanged: SynthBridge.ampRelease = value
                }
            }
        }

        // Filter Envelope
        GroupBox {
            title: "Filter Envelope"
            Layout.fillWidth: true
            ColumnLayout {
                spacing: 8
                RowLayout {
                    spacing: 12
                    RotarySlider {
                        label: "Base"
                        from: 20; to: 5000; value: SynthBridge.filterEnvBase
                        stepSize: 10; decimals: 0
                        onValueChanged: SynthBridge.filterEnvBase = value
                    }
                    RotarySlider {
                        label: "Amount"
                        from: 0; to: 5000; value: SynthBridge.filterEnvAmount
                        stepSize: 10; decimals: 0
                        onValueChanged: SynthBridge.filterEnvAmount = value
                    }
                }
                RowLayout {
                    spacing: 12
                    RotarySlider {
                        label: "Attack"
                        from: 0.001; to: 5; value: SynthBridge.filterEnvAttack
                        stepSize: 0.01; decimals: 2
                        onValueChanged: SynthBridge.filterEnvAttack = value
                    }
                    RotarySlider {
                        label: "Decay"
                        from: 0.001; to: 5; value: SynthBridge.filterEnvDecay
                        stepSize: 0.01; decimals: 2
                        onValueChanged: SynthBridge.filterEnvDecay = value
                    }
                    RotarySlider {
                        label: "Sustain"
                        from: 0; to: 1; value: SynthBridge.filterEnvSustain
                        onValueChanged: SynthBridge.filterEnvSustain = value
                    }
                    RotarySlider {
                        label: "Release"
                        from: 0.001; to: 5; value: SynthBridge.filterEnvRelease
                        stepSize: 0.01; decimals: 2
                        onValueChanged: SynthBridge.filterEnvRelease = value
                    }
                }
            }
        }

        // Attack Envelope
        GroupBox {
            title: "Attack Envelope (FM/AM Burst)"
            Layout.fillWidth: true
            RowLayout {
                spacing: 12
                RotarySlider {
                    label: "Attack"
                    from: 0.001; to: 1; value: SynthBridge.atkEnvAttack
                    stepSize: 0.001; decimals: 3
                    onValueChanged: SynthBridge.atkEnvAttack = value
                }
                RotarySlider {
                    label: "Decay"
                    from: 0.001; to: 2; value: SynthBridge.atkEnvDecay
                    stepSize: 0.01; decimals: 2
                    onValueChanged: SynthBridge.atkEnvDecay = value
                }
                RotarySlider {
                    label: "Sustain"
                    from: 0; to: 1; value: SynthBridge.atkEnvSustain
                    onValueChanged: SynthBridge.atkEnvSustain = value
                }
                RotarySlider {
                    label: "Release"
                    from: 0.001; to: 2; value: SynthBridge.atkEnvRelease
                    stepSize: 0.01; decimals: 2
                    onValueChanged: SynthBridge.atkEnvRelease = value
                }
            }
        }
    }
}
