import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Sub37

GroupBox {
    title: "MODULATION"
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

        // Pitch LFO 1 (Osc 1 + Sub)
        ColumnLayout {
            spacing: 2
            Label {
                text: "Pitch LFO 1"
                color: "#b0b0b0"
                font.pixelSize: 10
                font.weight: Font.Medium
                Layout.alignment: Qt.AlignHCenter
            }
            RowLayout {
                spacing: 8
                RotarySlider {
                    label: "Rate"
                    from: 0.1; to: 20; value: SynthBridge.pitchLfoRate
                    stepSize: 0.1; decimals: 1
                    onValueChanged: SynthBridge.pitchLfoRate = value
                }
                RotarySlider {
                    label: "Depth"
                    from: 0; to: 50; value: SynthBridge.pitchLfoDepth
                    stepSize: 0.1; decimals: 1
                    onValueChanged: SynthBridge.pitchLfoDepth = value
                }
                ColumnLayout {
                    spacing: 1
                    Repeater {
                        model: ["Sine", "Square", "Saw", "InvSaw", "Tri"]
                        SmallRadioButton {
                            checked: SynthBridge.pitchLfoShape === index
                            onClicked: SynthBridge.pitchLfoShape = index
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

        // Pitch LFO 2 (Osc 2 only)
        ColumnLayout {
            spacing: 2
            Label {
                text: "Pitch LFO 2"
                color: "#b0b0b0"
                font.pixelSize: 10
                font.weight: Font.Medium
                Layout.alignment: Qt.AlignHCenter
            }
            RowLayout {
                spacing: 8
                RotarySlider {
                    label: "Rate"
                    from: 0.1; to: 20; value: SynthBridge.pitchLfo2Rate
                    stepSize: 0.1; decimals: 1
                    onValueChanged: SynthBridge.pitchLfo2Rate = value
                }
                RotarySlider {
                    label: "Depth"
                    from: 0; to: 50; value: SynthBridge.pitchLfo2Depth
                    stepSize: 0.1; decimals: 1
                    onValueChanged: SynthBridge.pitchLfo2Depth = value
                }
                ColumnLayout {
                    spacing: 1
                    Repeater {
                        model: ["Sine", "Square", "Saw", "InvSaw", "Tri"]
                        SmallRadioButton {
                            checked: SynthBridge.pitchLfo2Shape === index
                            onClicked: SynthBridge.pitchLfo2Shape = index
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

        // Filter LFO
        ColumnLayout {
            spacing: 2
            Label {
                text: "Filter LFO"
                color: "#b0b0b0"
                font.pixelSize: 10
                font.weight: Font.Medium
                Layout.alignment: Qt.AlignHCenter
            }
            RowLayout {
                spacing: 8
                RotarySlider {
                    label: "Rate"
                    from: 0.1; to: 20; value: SynthBridge.filterLfoRate
                    stepSize: 0.1; decimals: 1
                    onValueChanged: SynthBridge.filterLfoRate = value
                }
                RotarySlider {
                    label: "Depth"
                    from: 0; to: 5000; value: SynthBridge.filterLfoDepth
                    stepSize: 10; decimals: 0
                    onValueChanged: SynthBridge.filterLfoDepth = value
                }
                ColumnLayout {
                    spacing: 1
                    Repeater {
                        model: ["Sine", "Square", "Saw", "InvSaw", "Tri"]
                        SmallRadioButton {
                            checked: SynthBridge.filterLfoShape === index
                            onClicked: SynthBridge.filterLfoShape = index
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

        // PWM LFO
        ColumnLayout {
            spacing: 2
            Label {
                text: "PWM LFO"
                color: "#b0b0b0"
                font.pixelSize: 10
                font.weight: Font.Medium
                Layout.alignment: Qt.AlignHCenter
            }
            RowLayout {
                spacing: 8
                RotarySlider {
                    label: "Rate"
                    from: 0.1; to: 20; value: SynthBridge.pwmDcLfoRate
                    stepSize: 0.1; decimals: 1
                    onValueChanged: SynthBridge.pwmDcLfoRate = value
                }
                RotarySlider {
                    label: "Depth"
                    from: 0; to: 0.5; value: SynthBridge.pwmDcLfoDepth
                    stepSize: 0.01; decimals: 2
                    onValueChanged: SynthBridge.pwmDcLfoDepth = value
                }
                ColumnLayout {
                    spacing: 1
                    Repeater {
                        model: ["Sine", "Square", "Saw", "InvSaw", "Tri"]
                        SmallRadioButton {
                            checked: SynthBridge.pwmDcLfoShape === index
                            onClicked: SynthBridge.pwmDcLfoShape = index
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

        // Feedback Delay
        ColumnLayout {
            spacing: 2
            Label {
                text: "Delay"
                color: "#b0b0b0"
                font.pixelSize: 10
                font.weight: Font.Medium
                Layout.alignment: Qt.AlignHCenter
            }
            RowLayout {
                spacing: 8
                RotarySlider {
                    label: "Time"
                    from: 0; to: 1; value: SynthBridge.delayTime
                    stepSize: 0.001; decimals: 3
                    onValueChanged: SynthBridge.delayTime = value
                }
                RotarySlider {
                    label: "Fdbk"
                    from: 0; to: 0.95; value: SynthBridge.feedback
                    stepSize: 0.01; decimals: 2
                    onValueChanged: SynthBridge.feedback = value
                }
            }
        }

        Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: "#333333" }

        // Glide
        ColumnLayout {
            spacing: 2
            Label {
                text: "Glide"
                color: "#b0b0b0"
                font.pixelSize: 10
                font.weight: Font.Medium
                Layout.alignment: Qt.AlignHCenter
            }
            RotarySlider {
                label: "Time"
                from: 0; to: 2; value: SynthBridge.glideTime
                stepSize: 0.01; decimals: 2
                onValueChanged: SynthBridge.glideTime = value
            }
        }
    }
}
