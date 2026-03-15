import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Sub37

ApplicationWindow {
    id: window
    visible: true
    width: 860
    height: 640
    title: "Sub 37 — Paraphonic Analog Synthesizer"
    color: "#181818"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 0
        spacing: 0

        // Header bar — Moog-style branding
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 36
            color: "#0d0d0d"

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 16
                anchors.rightMargin: 16

                Label {
                    text: "SUB 37"
                    font.pixelSize: 16
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

            // Bottom accent line
            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: 1
                color: "#333333"
            }
        }

        // Main panel area
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 10
            spacing: 8

            TabBar {
                id: tabBar
                Layout.fillWidth: true
                background: Rectangle { color: "#141414" }

                Repeater {
                    model: ["OSCILLATORS", "FILTER", "AMP ENVELOPE", "MODULATION"]
                    TabButton {
                        text: modelData
                        font.pixelSize: 10
                        font.family: "Helvetica Neue"
                        font.letterSpacing: 1.5
                        font.weight: Font.Medium

                        background: Rectangle {
                            color: parent.checked ? "#2a2a2a" : "#141414"
                            Rectangle {
                                anchors.bottom: parent.bottom
                                width: parent.width
                                height: 2
                                color: parent.parent.checked ? "#d4a017" : "transparent"
                            }
                        }
                        contentItem: Label {
                            text: parent.text
                            font: parent.font
                            color: parent.checked ? "#e0e0e0" : "#666666"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }
            }

            // Panel background
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "#1c1c1c"
                radius: 2
                border.color: "#2a2a2a"
                border.width: 1

                StackLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    currentIndex: tabBar.currentIndex

                    OscillatorPanel {}
                    FilterPanel {}
                    EnvelopePanel {}
                    ModulationPanel {}
                }
            }
        }

        // Piano keyboard — dark walnut-style surround
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 130
            Layout.leftMargin: 10
            Layout.rightMargin: 10
            Layout.bottomMargin: 10
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
