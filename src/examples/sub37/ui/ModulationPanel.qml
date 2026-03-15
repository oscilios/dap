import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Sub37

ScrollView {
    contentWidth: availableWidth

    ColumnLayout {
        width: parent.width
        spacing: 16

        // Mod 1: Pitch LFO
        GroupBox {
            title: "Mod 1 — Pitch LFO"
            Layout.fillWidth: true
            ColumnLayout {
                spacing: 8
                RowLayout {
                    spacing: 12
                    RotarySlider {
                        label: "Rate (Hz)"
                        from: 0.1; to: 20; value: SynthBridge.pitchLfoRate
                        stepSize: 0.1; decimals: 1
                        onValueChanged: SynthBridge.pitchLfoRate = value
                    }
                    RotarySlider {
                        label: "Depth (Hz)"
                        from: 0; to: 50; value: SynthBridge.pitchLfoDepth
                        stepSize: 0.1; decimals: 1
                        onValueChanged: SynthBridge.pitchLfoDepth = value
                    }
                }
                RowLayout {
                    spacing: 8
                    Label { text: "Shape:"; color: "#cccccc"; font.pixelSize: 12 }
                    Repeater {
                        model: ["Sine", "Square", "Saw", "InvSaw", "Triangle"]
                        RadioButton {
                            text: modelData
                            checked: SynthBridge.pitchLfoShape === index
                            onClicked: SynthBridge.pitchLfoShape = index
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

        // Mod 2: Filter LFO
        GroupBox {
            title: "Mod 2 — Filter LFO"
            Layout.fillWidth: true
            ColumnLayout {
                spacing: 8
                RowLayout {
                    spacing: 12
                    RotarySlider {
                        label: "Rate (Hz)"
                        from: 0.1; to: 20; value: SynthBridge.filterLfoRate
                        stepSize: 0.1; decimals: 1
                        onValueChanged: SynthBridge.filterLfoRate = value
                    }
                    RotarySlider {
                        label: "Depth (Hz)"
                        from: 0; to: 5000; value: SynthBridge.filterLfoDepth
                        stepSize: 10; decimals: 0
                        onValueChanged: SynthBridge.filterLfoDepth = value
                    }
                }
                RowLayout {
                    spacing: 8
                    Label { text: "Shape:"; color: "#cccccc"; font.pixelSize: 12 }
                    Repeater {
                        model: ["Sine", "Square", "Saw", "InvSaw", "Triangle"]
                        RadioButton {
                            text: modelData
                            checked: SynthBridge.filterLfoShape === index
                            onClicked: SynthBridge.filterLfoShape = index
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

        // Glide
        GroupBox {
            title: "Glide"
            Layout.fillWidth: true
            RowLayout {
                spacing: 12
                RotarySlider {
                    label: "Time (s)"
                    from: 0; to: 2; value: SynthBridge.glideTime
                    stepSize: 0.01; decimals: 2
                    onValueChanged: SynthBridge.glideTime = value
                }
            }
        }
    }
}
