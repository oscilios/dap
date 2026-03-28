import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Sub37

GroupBox {
    title: "ARPEGGIATOR"
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

        // On/Off + Hold toggles
        ColumnLayout {
            spacing: 4
            Layout.alignment: Qt.AlignVCenter

            Switch {
                id: arpSwitch
                checked: SynthBridge.arpEnabled
                onCheckedChanged: SynthBridge.arpEnabled = checked

                indicator: Rectangle {
                    implicitWidth: 36
                    implicitHeight: 18
                    radius: 9
                    color: arpSwitch.checked ? "#d4a017" : "#333333"
                    border.color: arpSwitch.checked ? "#d4a017" : "#555555"

                    Rectangle {
                        x: arpSwitch.checked ? parent.width - width - 2 : 2
                        y: 2
                        width: 14
                        height: 14
                        radius: 7
                        color: "#e0e0e0"
                        Behavior on x { NumberAnimation { duration: 100 } }
                    }
                }
            }

            Label {
                text: arpSwitch.checked ? "ON" : "OFF"
                color: arpSwitch.checked ? "#d4a017" : "#666666"
                font.pixelSize: 9
                font.weight: Font.Medium
                Layout.alignment: Qt.AlignHCenter
            }

            Switch {
                id: holdSwitch
                checked: SynthBridge.hold
                onCheckedChanged: SynthBridge.hold = checked

                indicator: Rectangle {
                    implicitWidth: 36
                    implicitHeight: 18
                    radius: 9
                    color: holdSwitch.checked ? "#4fc3f7" : "#333333"
                    border.color: holdSwitch.checked ? "#4fc3f7" : "#555555"

                    Rectangle {
                        x: holdSwitch.checked ? parent.width - width - 2 : 2
                        y: 2
                        width: 14
                        height: 14
                        radius: 7
                        color: "#e0e0e0"
                        Behavior on x { NumberAnimation { duration: 100 } }
                    }
                }
            }

            Label {
                text: "HOLD"
                color: holdSwitch.checked ? "#4fc3f7" : "#666666"
                font.pixelSize: 9
                font.weight: Font.Medium
                Layout.alignment: Qt.AlignHCenter
            }
        }

        Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: "#333333" }

        // BPM + Gate
        ColumnLayout {
            spacing: 2
            Label {
                text: "Tempo"
                color: "#b0b0b0"
                font.pixelSize: 10
                font.weight: Font.Medium
                Layout.alignment: Qt.AlignHCenter
            }
            RowLayout {
                spacing: 8
                RotarySlider {
                    label: "BPM"
                    from: 40; to: 300; value: SynthBridge.arpBpm
                    stepSize: 1; decimals: 0
                    onValueChanged: SynthBridge.arpBpm = value
                }
                RotarySlider {
                    label: "Gate"
                    from: 0.1; to: 1.0; value: SynthBridge.arpGate
                    stepSize: 0.05; decimals: 2
                    onValueChanged: SynthBridge.arpGate = value
                }
            }
        }

        Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: "#333333" }

        // Pattern
        ColumnLayout {
            spacing: 2
            Label {
                text: "Pattern"
                color: "#b0b0b0"
                font.pixelSize: 10
                font.weight: Font.Medium
                Layout.alignment: Qt.AlignHCenter
            }
            ColumnLayout {
                spacing: 1
                Repeater {
                    model: ["Up", "Down", "Up/Down", "Random"]
                    SmallRadioButton {
                        checked: SynthBridge.arpPattern === index
                        onClicked: SynthBridge.arpPattern = index
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

        Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: "#333333" }

        // Subdivision
        ColumnLayout {
            spacing: 2
            Label {
                text: "Division"
                color: "#b0b0b0"
                font.pixelSize: 10
                font.weight: Font.Medium
                Layout.alignment: Qt.AlignHCenter
            }
            ColumnLayout {
                spacing: 1
                Repeater {
                    model: [
                        { label: "1/4", value: 1 },
                        { label: "1/8", value: 2 },
                        { label: "1/8T", value: 3 },
                        { label: "1/16", value: 4 }
                    ]
                    SmallRadioButton {
                        checked: SynthBridge.arpSubdivision === modelData.value
                        onClicked: SynthBridge.arpSubdivision = modelData.value
                        contentItem: Label {
                            text: modelData.label
                            color: parent.checked ? "#e0e0e0" : "#777777"
                            font.pixelSize: 10
                            leftPadding: parent.indicator.width + 2
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }
            }
        }

        Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: "#333333" }

        // Octave range
        RotarySlider {
            label: "Octaves"
            from: 1; to: 4; value: SynthBridge.arpOctaveRange
            stepSize: 1; decimals: 0
            onValueChanged: SynthBridge.arpOctaveRange = value
        }
    }
}
