import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Sub37

GroupBox {
    title: "SEQUENCER"
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

        // Transport controls
        ColumnLayout {
            spacing: 4
            Layout.alignment: Qt.AlignVCenter

            Button {
                id: playBtn
                implicitWidth: 36
                implicitHeight: 36

                background: Rectangle {
                    radius: 4
                    color: SynthBridge.seqPlaying ? "#d4a017" : "#333333"
                    border.color: SynthBridge.seqPlaying ? "#d4a017" : "#555555"
                }

                contentItem: Label {
                    text: SynthBridge.seqPlaying ? "\u25A0" : "\u25B6"
                    font.pixelSize: 16
                    color: "#e0e0e0"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: SynthBridge.seqPlaying = !SynthBridge.seqPlaying
            }

            Button {
                implicitWidth: 36
                implicitHeight: 20

                background: Rectangle {
                    radius: 3
                    color: "#333333"
                    border.color: "#555555"
                }

                contentItem: Label {
                    text: "CLR"
                    font.pixelSize: 8
                    font.weight: Font.Medium
                    color: "#b0b0b0"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: SynthBridge.seqClear()
            }
        }

        Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: "#333333" }

        // Tempo controls
        RowLayout {
            spacing: 8
            RotarySlider {
                label: "BPM"
                from: 40; to: 300; value: SynthBridge.seqBpm
                stepSize: 1; decimals: 0
                onValueChanged: SynthBridge.seqBpm = value
            }
            RotarySlider {
                label: "Steps"
                from: 1; to: 32; value: SynthBridge.seqStepCount
                stepSize: 1; decimals: 0
                onValueChanged: SynthBridge.seqStepCount = value
            }
            RotarySlider {
                label: "Gate"
                from: 0.1; to: 1.0; value: SynthBridge.seqGate
                stepSize: 0.05; decimals: 2
                onValueChanged: SynthBridge.seqGate = value
            }

            // Subdivision
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
                        checked: SynthBridge.seqSubdivision === modelData.value
                        onClicked: SynthBridge.seqSubdivision = modelData.value
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
    }
}
