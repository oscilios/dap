import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Sub37

RowLayout {
    spacing: 8

    // ── Oscillators section ──
    GroupBox {
        title: "OSCILLATORS"
        Layout.fillWidth: true
        Layout.fillHeight: true
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
            spacing: 10

            // Osc 1
            ColumnLayout {
                spacing: 2
                Label {
                    text: "Osc 1"
                    color: "#b0b0b0"
                    font.pixelSize: 10
                    font.weight: Font.Medium
                    Layout.alignment: Qt.AlignHCenter
                }
                RowLayout {
                    spacing: 8
                    RotarySlider {
                        label: "Level"
                        from: 0; to: 1; value: SynthBridge.osc1Level
                        onValueChanged: SynthBridge.osc1Level = value
                    }
                    ColumnLayout {
                        spacing: 1
                        Repeater {
                            model: ["Sine", "Square", "Saw", "InvSaw", "Tri"]
                            SmallRadioButton {
                                checked: SynthBridge.osc1Shape === index
                                onClicked: SynthBridge.osc1Shape = index
                                contentItem: Label {
                                    text: modelData
                                    color: parent.checked ? "#e0e0e0" : "#777777"
                                    font.pixelSize: 10
                                    leftPadding: parent.indicator.width + 2
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }
                        }
                    }
                }
            }

            Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: "#333333" }

            // Osc 2
            ColumnLayout {
                spacing: 2
                Label {
                    text: "Osc 2"
                    color: "#b0b0b0"
                    font.pixelSize: 10
                    font.weight: Font.Medium
                    Layout.alignment: Qt.AlignHCenter
                }
                RowLayout {
                    spacing: 8
                    RotarySlider {
                        label: "Level"
                        from: 0; to: 1; value: SynthBridge.osc2Level
                        onValueChanged: SynthBridge.osc2Level = value
                    }
                    ColumnLayout {
                        spacing: 1
                        Repeater {
                            model: ["Sine", "Square", "Saw", "InvSaw", "Tri"]
                            SmallRadioButton {
                                checked: SynthBridge.osc2Shape === index
                                onClicked: SynthBridge.osc2Shape = index
                                contentItem: Label {
                                    text: modelData
                                    color: parent.checked ? "#e0e0e0" : "#777777"
                                    font.pixelSize: 10
                                    leftPadding: parent.indicator.width + 2
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }
                        }
                    }
                    RotarySlider {
                        label: "Octave"
                        from: -2; to: 2; value: SynthBridge.osc2Octave
                        stepSize: 1; decimals: 0
                        onValueChanged: SynthBridge.osc2Octave = value
                    }
                    RotarySlider {
                        label: "Beat"
                        from: -10; to: 10; value: SynthBridge.osc2BeatFreq
                        stepSize: 0.1; decimals: 1
                        onValueChanged: SynthBridge.osc2BeatFreq = value
                    }
                }
            }

            Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: "#333333" }

            // Sub
            ColumnLayout {
                spacing: 2
                Label {
                    text: "Sub"
                    color: "#b0b0b0"
                    font.pixelSize: 10
                    font.weight: Font.Medium
                    Layout.alignment: Qt.AlignHCenter
                }
                RotarySlider {
                    label: "Level"
                    from: 0; to: 1; value: SynthBridge.subLevel
                    onValueChanged: SynthBridge.subLevel = value
                }
            }

            Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: "#333333" }

            // Noise
            ColumnLayout {
                spacing: 2
                Label {
                    text: "Noise"
                    color: "#b0b0b0"
                    font.pixelSize: 10
                    font.weight: Font.Medium
                    Layout.alignment: Qt.AlignHCenter
                }
                RowLayout {
                    spacing: 8
                    RotarySlider {
                        label: "Gain"
                        from: 0; to: 1; value: SynthBridge.noiseGain
                        onValueChanged: SynthBridge.noiseGain = value
                    }
                    ColumnLayout {
                        spacing: 1
                        Repeater {
                            model: ["White", "Pink", "Brown", "1/f\u00B3"]
                            SmallRadioButton {
                                checked: SynthBridge.noiseColor === index
                                onClicked: SynthBridge.noiseColor = index
                                contentItem: Label {
                                    text: modelData
                                    color: parent.checked ? "#e0e0e0" : "#777777"
                                    font.pixelSize: 10
                                    leftPadding: parent.indicator.width + 2
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // ── Filter section ──
    FilterPanel {
        Layout.fillHeight: true
    }
}
