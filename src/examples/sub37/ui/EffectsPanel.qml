import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Sub37

RowLayout {
    spacing: 8

    // --- Distortion ---
    GroupBox {
        title: "DISTORTION"
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

            ColumnLayout {
                spacing: 2
                Label {
                    text: "Controls"
                    color: "#b0b0b0"
                    font.pixelSize: 10
                    font.weight: Font.Medium
                    Layout.alignment: Qt.AlignHCenter
                }
                RowLayout {
                    spacing: 8
                    RotarySlider {
                        label: "Drive"
                        from: 1; to: 50; value: SynthBridge.distDrive
                        stepSize: 0.1; decimals: 1
                        onValueChanged: SynthBridge.distDrive = value
                    }
                    RotarySlider {
                        label: "Mix"
                        from: 0; to: 1; value: SynthBridge.distMix
                        onValueChanged: SynthBridge.distMix = value
                    }
                    RotarySlider {
                        label: "Param"
                        from: 0.1; to: 16; value: SynthBridge.distParam
                        stepSize: 0.1; decimals: 1
                        onValueChanged: SynthBridge.distParam = value
                    }
                }
            }

            Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: "#333333" }

            ColumnLayout {
                spacing: 1
                Label {
                    text: "Type"
                    color: "#b0b0b0"
                    font.pixelSize: 10
                    font.weight: Font.Medium
                    Layout.alignment: Qt.AlignHCenter
                    Layout.bottomMargin: 2
                }
                Repeater {
                    model: ["Hard Clip", "Soft Clip", "Wavefold", "Exponential", "Bitcrush"]
                    SmallRadioButton {
                        checked: SynthBridge.distType === index
                        onClicked: SynthBridge.distType = index
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

    // --- Flanger ---
    GroupBox {
        title: "FLANGER"
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
            spacing: 8
            RotarySlider {
                label: "Delay"
                from: 0.001; to: 0.020; value: SynthBridge.flangerDelay
                stepSize: 0.001; decimals: 3
                onValueChanged: SynthBridge.flangerDelay = value
            }
            RotarySlider {
                label: "Depth"
                from: 0; to: 0.010; value: SynthBridge.flangerDepth
                stepSize: 0.0005; decimals: 4
                onValueChanged: SynthBridge.flangerDepth = value
            }
            RotarySlider {
                label: "Rate"
                from: 0.05; to: 10; value: SynthBridge.flangerRate
                stepSize: 0.05; decimals: 2
                onValueChanged: SynthBridge.flangerRate = value
            }
            RotarySlider {
                label: "Fdbk"
                from: -0.95; to: 0.95; value: SynthBridge.flangerFeedback
                stepSize: 0.01; decimals: 2
                onValueChanged: SynthBridge.flangerFeedback = value
            }
            RotarySlider {
                label: "Wet"
                from: 0; to: 1; value: SynthBridge.flangerWet
                onValueChanged: SynthBridge.flangerWet = value
            }
        }
    }
}
