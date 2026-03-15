import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ComplexFM

ApplicationWindow {
    id: window
    visible: true
    width: 800
    height: 650
    title: "Complex FM Synth"
    color: "#1e1e1e"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        TabBar {
            id: tabBar
            Layout.fillWidth: true

            TabButton { text: "Operators" }
            TabButton { text: "Envelopes" }
            TabButton { text: "Filter & EQ" }
            TabButton { text: "Effects" }
        }

        StackLayout {
            currentIndex: tabBar.currentIndex
            Layout.fillWidth: true
            Layout.fillHeight: true

            // Operators tab with sub-tabs
            ColumnLayout {
                spacing: 4

                TabBar {
                    id: opTabBar
                    Layout.fillWidth: true
                    Repeater {
                        model: 5
                        TabButton { text: "Op " + (index + 1) }
                    }
                }

                StackLayout {
                    currentIndex: opTabBar.currentIndex
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Repeater {
                        model: 5
                        OperatorPanel {
                            required property int index
                            readonly property string p: "op" + index + "_"
                            opIndex: index
                            carrierLevel: SynthBridge[p + "carrierLevel"]
                            amRate: SynthBridge[p + "amRate"]
                            amDepth: SynthBridge[p + "amDepth"]
                            amEnvAmount: SynthBridge[p + "amEnvAmount"]
                            vibratoRate: SynthBridge[p + "vibratoRate"]
                            vibratoDepth: SynthBridge[p + "vibratoDepth"]
                            modFreq: SynthBridge[p + "modFreq"]
                            modIdx: SynthBridge[p + "modIdx"]
                            fmEnvAmount: SynthBridge[p + "fmEnvAmount"]
                            busGain: SynthBridge[p + "busGain"]
                            onCarrierLevelEdited: (v) => SynthBridge[p + "carrierLevel"] = v
                            onAmRateEdited: (v) => SynthBridge[p + "amRate"] = v
                            onAmDepthEdited: (v) => SynthBridge[p + "amDepth"] = v
                            onAmEnvAmountEdited: (v) => SynthBridge[p + "amEnvAmount"] = v
                            onVibratoRateEdited: (v) => SynthBridge[p + "vibratoRate"] = v
                            onVibratoDepthEdited: (v) => SynthBridge[p + "vibratoDepth"] = v
                            onModFreqEdited: (v) => SynthBridge[p + "modFreq"] = v
                            onModIdxEdited: (v) => SynthBridge[p + "modIdx"] = v
                            onFmEnvAmountEdited: (v) => SynthBridge[p + "fmEnvAmount"] = v
                            onBusGainEdited: (v) => SynthBridge[p + "busGain"] = v
                        }
                    }
                }
            }

            EnvelopePanel {}
            FilterPanel {}
            EffectsPanel {}
        }

        // Piano keyboard always visible at the bottom
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 130
            color: "#2a2a2a"
            radius: 4

            PianoKeyboard {
                anchors.fill: parent
                anchors.margins: 4
            }
        }
    }

    Component.onCompleted: SynthBridge.startAudio()
    Component.onDestruction: SynthBridge.stopAudio()
}
