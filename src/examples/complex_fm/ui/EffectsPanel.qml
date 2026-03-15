import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ComplexFM

ScrollView {
    contentWidth: availableWidth

    ColumnLayout {
        width: parent.width
        spacing: 16

        // Phaser
        GroupBox {
            title: "Phaser"
            Layout.fillWidth: true
            RowLayout {
                spacing: 12
                RotarySlider {
                    label: "Rate"
                    from: 0.01; to: 10; value: SynthBridge.phaserRate
                    stepSize: 0.01; decimals: 2
                    onValueChanged: SynthBridge.phaserRate = value
                }
                RotarySlider {
                    label: "Depth"
                    from: 0; to: 1; value: SynthBridge.phaserDepth
                    onValueChanged: SynthBridge.phaserDepth = value
                }
                RotarySlider {
                    label: "Feedback"
                    from: 0; to: 0.99; value: SynthBridge.phaserFeedback
                    onValueChanged: SynthBridge.phaserFeedback = value
                }
                RotarySlider {
                    label: "Wet"
                    from: 0; to: 1; value: SynthBridge.phaserWet
                    onValueChanged: SynthBridge.phaserWet = value
                }
            }
        }

        // Noise
        GroupBox {
            title: "Noise"
            Layout.fillWidth: true
            ColumnLayout {
                spacing: 8
                RotarySlider {
                    label: "Gain"
                    from: 0; to: 1; value: SynthBridge.noiseGain
                    onValueChanged: SynthBridge.noiseGain = value
                }
                RowLayout {
                    spacing: 8
                    Label { text: "Color:"; color: "#cccccc"; font.pixelSize: 12 }
                    Repeater {
                        model: ["White", "Pink", "Brown", "1/f^3"]
                        RadioButton {
                            text: modelData
                            checked: SynthBridge.noiseColor === index
                            onClicked: SynthBridge.noiseColor = index
                            contentItem: Label {
                                text: parent.text
                                color: "#cccccc"
                                font.pixelSize: 11
                                leftPadding: parent.indicator.width + 4
                            }
                        }
                    }
                }
            }
        }
    }
}
