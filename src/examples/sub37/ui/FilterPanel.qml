import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Sub37

GroupBox {
    title: "FILTER"
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

        // Ladder Filter
        ColumnLayout {
            spacing: 2
            Label {
                text: "Ladder"
                color: "#b0b0b0"
                font.pixelSize: 10
                font.weight: Font.Medium
                Layout.alignment: Qt.AlignHCenter
            }
            RowLayout {
                spacing: 8
                RotarySlider {
                    id: rangeSlider
                    label: "Range"
                    from: 200; to: 20000; value: 2000
                    stepSize: 100; decimals: 0
                }
                RotarySlider {
                    label: "Cutoff"
                    from: 20; to: rangeSlider.value; value: SynthBridge.filterCutoff
                    stepSize: Math.max(1, rangeSlider.value / 200); decimals: 0
                    onValueChanged: SynthBridge.filterCutoff = value
                }
                RotarySlider {
                    label: "Reso"
                    from: 0; to: 1; value: SynthBridge.filterResonance
                    onValueChanged: SynthBridge.filterResonance = value
                }
                RotarySlider {
                    label: "Env Amt"
                    from: 0; to: 10000; value: SynthBridge.filterEnvAmount
                    stepSize: 10; decimals: 0
                    onValueChanged: SynthBridge.filterEnvAmount = value
                }
            }
        }

        Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: "#333333" }

        // Filter Envelope
        ColumnLayout {
            spacing: 2
            Label {
                text: "Filter Envelope"
                color: "#b0b0b0"
                font.pixelSize: 10
                font.weight: Font.Medium
                Layout.alignment: Qt.AlignHCenter
            }
            RowLayout {
                spacing: 8
                RotarySlider {
                    label: "A"
                    from: 0.001; to: 5; value: SynthBridge.filterAttack
                    stepSize: 0.001; decimals: 3
                    onValueChanged: SynthBridge.filterAttack = value
                }
                RotarySlider {
                    label: "D"
                    from: 0.01; to: 10; value: SynthBridge.filterDecay
                    stepSize: 0.01; decimals: 2
                    onValueChanged: SynthBridge.filterDecay = value
                }
                RotarySlider {
                    label: "S"
                    from: 0; to: 1; value: SynthBridge.filterSustain
                    onValueChanged: SynthBridge.filterSustain = value
                }
                RotarySlider {
                    label: "R"
                    from: 0.01; to: 10; value: SynthBridge.filterRelease
                    stepSize: 0.01; decimals: 2
                    onValueChanged: SynthBridge.filterRelease = value
                }
            }
        }
    }
}
