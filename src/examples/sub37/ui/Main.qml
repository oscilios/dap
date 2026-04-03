import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import Sub37

ApplicationWindow {
    id: window
    visible: true
    width: 1440
    height: 758
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
                spacing: 8

                Label {
                    text: "SUB 37"
                    font.pixelSize: 14
                    font.family: "Helvetica Neue"
                    font.weight: Font.Bold
                    font.letterSpacing: 4
                    color: "#c8c8c8"
                }

                // --- Preset selector ---
                Rectangle {
                    Layout.leftMargin: 24
                    width: 180
                    height: 22
                    radius: 3
                    color: "#252525"
                    border.color: presetCombo.popup.visible ? "#888" : "#444"
                    border.width: 1

                    ComboBox {
                        id: presetCombo
                        anchors.fill: parent
                        model: SynthBridge.presetList
                        font.pixelSize: 11
                        font.family: "Helvetica Neue"
                        currentIndex: {
                            var idx = model ? model.indexOf(SynthBridge.currentPreset) : -1;
                            return idx >= 0 ? idx : -1;
                        }

                        onActivated: (index) => {
                            SynthBridge.loadPreset(model[index]);
                        }

                        background: Item {}
                        contentItem: Text {
                            leftPadding: 8
                            text: presetCombo.currentIndex >= 0 ? presetCombo.currentText : "— Init —"
                            color: "#c8c8c8"
                            font: presetCombo.font
                            verticalAlignment: Text.AlignVCenter
                            elide: Text.ElideRight
                        }
                        indicator: Text {
                            x: parent.width - width - 6
                            anchors.verticalCenter: parent.verticalCenter
                            text: "\u25BE"
                            color: "#888"
                            font.pixelSize: 10
                        }

                        popup: Popup {
                            y: presetCombo.height
                            width: presetCombo.width
                            implicitHeight: contentItem.implicitHeight + 2
                            padding: 1

                            background: Rectangle {
                                color: "#2a2a2a"
                                border.color: "#555"
                                border.width: 1
                                radius: 3
                            }

                            contentItem: ListView {
                                clip: true
                                implicitHeight: contentHeight
                                model: presetCombo.popup.visible ? presetCombo.delegateModel : null
                                currentIndex: presetCombo.highlightedIndex
                            }
                        }

                        delegate: ItemDelegate {
                            width: presetCombo.width - 2
                            height: 24
                            contentItem: Text {
                                text: modelData
                                color: highlighted ? "#fff" : "#c8c8c8"
                                font.pixelSize: 11
                                font.family: "Helvetica Neue"
                                verticalAlignment: Text.AlignVCenter
                                leftPadding: 6
                            }
                            highlighted: presetCombo.highlightedIndex === index
                            background: Rectangle {
                                color: highlighted ? "#555" : "transparent"
                            }
                        }
                    }
                }

                // Save button
                Rectangle {
                    width: 48
                    height: 22
                    radius: 3
                    color: saveMouseArea.containsMouse ? "#444" : "#303030"
                    border.color: "#555"
                    border.width: 1

                    Text {
                        anchors.centerIn: parent
                        text: "SAVE"
                        font.pixelSize: 9
                        font.family: "Helvetica Neue"
                        font.letterSpacing: 1
                        color: "#aaa"
                    }

                    MouseArea {
                        id: saveMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: saveDialog.open()
                    }
                }

                // Delete button
                Rectangle {
                    width: 48
                    height: 22
                    radius: 3
                    color: delMouseArea.containsMouse ? "#444" : "#303030"
                    border.color: "#555"
                    border.width: 1
                    opacity: SynthBridge.currentPreset.length > 0 ? 1.0 : 0.4

                    Text {
                        anchors.centerIn: parent
                        text: "DEL"
                        font.pixelSize: 9
                        font.family: "Helvetica Neue"
                        font.letterSpacing: 1
                        color: "#aaa"
                    }

                    MouseArea {
                        id: delMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        enabled: SynthBridge.currentPreset.length > 0
                        onClicked: {
                            SynthBridge.deletePreset(SynthBridge.currentPreset);
                        }
                    }
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

        // Save preset dialog
        Dialog {
            id: saveDialog
            anchors.centerIn: parent
            modal: true
            width: 300
            padding: 16

            background: Rectangle {
                color: "#2a2a2a"
                border.color: "#555"
                border.width: 1
                radius: 6
            }

            header: Item {
                height: 36
                Text {
                    anchors.centerIn: parent
                    text: "Save Preset"
                    color: "#c8c8c8"
                    font.pixelSize: 13
                    font.family: "Helvetica Neue"
                    font.weight: Font.Bold
                }
            }

            footer: Item {}

            ColumnLayout {
                anchors.fill: parent
                spacing: 12

                TextField {
                    id: presetNameField
                    Layout.fillWidth: true
                    placeholderText: "Preset name"
                    placeholderTextColor: "#666"
                    font.pixelSize: 13
                    font.family: "Helvetica Neue"
                    color: "#e0e0e0"
                    selectionColor: "#556"
                    background: Rectangle {
                        color: "#1a1a1a"
                        border.color: presetNameField.activeFocus ? "#888" : "#444"
                        border.width: 1
                        radius: 3
                    }
                    Keys.onReturnPressed: {
                        var name = text.trim();
                        if (name.length > 0) {
                            SynthBridge.savePreset(name);
                            saveDialog.close();
                        }
                    }
                    Keys.onEscapePressed: saveDialog.close()
                }

                RowLayout {
                    Layout.alignment: Qt.AlignRight
                    spacing: 8

                    Rectangle {
                        width: 64
                        height: 26
                        radius: 3
                        color: cancelMa.containsMouse ? "#444" : "#353535"
                        border.color: "#555"
                        border.width: 1
                        Text {
                            anchors.centerIn: parent
                            text: "Cancel"
                            color: "#aaa"
                            font.pixelSize: 11
                            font.family: "Helvetica Neue"
                        }
                        MouseArea {
                            id: cancelMa
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: saveDialog.close()
                        }
                    }

                    Rectangle {
                        width: 64
                        height: 26
                        radius: 3
                        color: okMa.containsMouse ? "#555" : "#484848"
                        border.color: "#666"
                        border.width: 1
                        Text {
                            anchors.centerIn: parent
                            text: "Save"
                            color: "#ddd"
                            font.pixelSize: 11
                            font.family: "Helvetica Neue"
                            font.weight: Font.Bold
                        }
                        MouseArea {
                            id: okMa
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: {
                                var name = presetNameField.text.trim();
                                if (name.length > 0) {
                                    SynthBridge.savePreset(name);
                                    saveDialog.close();
                                }
                            }
                        }
                    }
                }
            }

            onOpened: {
                presetNameField.text = SynthBridge.currentPreset;
                presetNameField.forceActiveFocus();
                presetNameField.selectAll();
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

            // Row 3: Arpeggiator + Sequencer
            RowLayout {
                Layout.fillWidth: true
                spacing: 8

                ArpPanel {
                    Layout.fillHeight: true
                }
                SequencerPanel {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
                PitchEnvPanel {
                    Layout.fillHeight: true
                }
            }
        }

        // Row 4: Step grid
        StepGrid {
            Layout.fillWidth: true
            Layout.preferredHeight: 32
            Layout.leftMargin: 8
            Layout.rightMargin: 8
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
