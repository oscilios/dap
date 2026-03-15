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

        // Pitch LFO
        ColumnLayout {
            spacing: 2
            Label {
                text: "Pitch LFO"
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
