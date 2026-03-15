import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Sub37

ScrollView {
    contentWidth: availableWidth

    ColumnLayout {
        width: parent.width
        spacing: 16

        // Osc 1
        GroupBox {
            title: "Oscillator 1"
            Layout.fillWidth: true
            ColumnLayout {
                spacing: 8
                RowLayout {
                    spacing: 12
                    RotarySlider {
                        label: "Level"
                        from: 0; to: 1; value: SynthBridge.osc1Level
                        onValueChanged: SynthBridge.osc1Level = value
                    }
                }
                RowLayout {
                    spacing: 8
                    Label { text: "Wave:"; color: "#cccccc"; font.pixelSize: 12 }
                    Repeater {
                        model: ["Sine", "Square", "Saw", "InvSaw", "Triangle"]
                        RadioButton {
                            text: modelData
                            checked: SynthBridge.osc1Shape === index
                            onClicked: SynthBridge.osc1Shape = index
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

        // Osc 2
        GroupBox {
            title: "Oscillator 2"
            Layout.fillWidth: true
            ColumnLayout {
                spacing: 8
                RowLayout {
                    spacing: 12
                    RotarySlider {
                        label: "Level"
                        from: 0; to: 1; value: SynthBridge.osc2Level
                        onValueChanged: SynthBridge.osc2Level = value
                    }
                    RotarySlider {
                        label: "Beat Freq"
                        from: -10; to: 10; value: SynthBridge.osc2BeatFreq
                        stepSize: 0.1; decimals: 1
                        onValueChanged: SynthBridge.osc2BeatFreq = value
                    }
                }
                RowLayout {
                    spacing: 8
                    Label { text: "Wave:"; color: "#cccccc"; font.pixelSize: 12 }
                    Repeater {
                        model: ["Sine", "Square", "Saw", "InvSaw", "Triangle"]
                        RadioButton {
                            text: modelData
                            checked: SynthBridge.osc2Shape === index
                            onClicked: SynthBridge.osc2Shape = index
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

        // Sub Oscillator
        GroupBox {
            title: "Sub Oscillator (Square, -1 Oct)"
            Layout.fillWidth: true
            RowLayout {
                spacing: 12
                RotarySlider {
                    label: "Level"
                    from: 0; to: 1; value: SynthBridge.subLevel
                    onValueChanged: SynthBridge.subLevel = value
                }
            }
        }

        // Noise
        GroupBox {
            title: "Noise"
            Layout.fillWidth: true
            ColumnLayout {
                spacing: 8
                RowLayout {
                    spacing: 12
                    RotarySlider {
                        label: "Gain"
                        from: 0; to: 1; value: SynthBridge.noiseGain
                        onValueChanged: SynthBridge.noiseGain = value
                    }
                }
                RowLayout {
                    spacing: 8
                    Label { text: "Color:"; color: "#cccccc"; font.pixelSize: 12 }
                    Repeater {
                        model: ["White", "Pink", "Brown", "1/f\u00B3"]
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
