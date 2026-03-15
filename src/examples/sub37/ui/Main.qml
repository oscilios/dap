import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Sub37

ApplicationWindow {
    id: window
    visible: true
    width: 1280
    height: 520
    title: "Sub 37 — Paraphonic Analog Synthesizer"
    color: "#181818"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 0
        spacing: 0

        // Header bar
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 32
            color: "#0d0d0d"

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 16
                anchors.rightMargin: 16

                Label {
                    text: "SUB 37"
                    font.pixelSize: 14
                    font.family: "Helvetica Neue"
                    font.weight: Font.Bold
                    font.letterSpacing: 4
                    color: "#c8c8c8"
                }
                Item { Layout.fillWidth: true }
                Label {
                    text: "PARAPHONIC ANALOG SYNTHESIZER"
                    font.pixelSize: 9
                    font.family: "Helvetica Neue"
                    font.letterSpacing: 2
                    color: "#666666"
                }
            }

            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: 1
                color: "#333333"
            }
        }

        // All controls
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 8
            spacing: 6

            // Row 1: Oscillators + Modulation
            OscillatorPanel {
                Layout.fillWidth: true
            }

            // Row 2: Modulation + Amp Envelope
            RowLayout {
                Layout.fillWidth: true
                spacing: 8

                ModulationPanel {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
                EnvelopePanel {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }

        // Piano keyboard
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 110
            Layout.leftMargin: 8
            Layout.rightMargin: 8
            Layout.bottomMargin: 8
            color: "#1a1510"
            radius: 4
            border.color: "#2a2520"
            border.width: 1

            PianoKeyboard {
                anchors.fill: parent
                anchors.margins: 6
            }
        }
    }

    Component.onCompleted: SynthBridge.startAudio()
    Component.onDestruction: SynthBridge.stopAudio()
}
