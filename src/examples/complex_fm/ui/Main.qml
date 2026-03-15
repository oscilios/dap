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

                    OperatorPanel {
                        opIndex: 0
                        carrierLevel: SynthBridge.op0_carrierLevel
                        amRate: SynthBridge.op0_amRate
                        amDepth: SynthBridge.op0_amDepth
                        amEnvAmount: SynthBridge.op0_amEnvAmount
                        vibratoRate: SynthBridge.op0_vibratoRate
                        vibratoDepth: SynthBridge.op0_vibratoDepth
                        modFreq: SynthBridge.op0_modFreq
                        modIdx: SynthBridge.op0_modIdx
                        fmEnvAmount: SynthBridge.op0_fmEnvAmount
                        busGain: SynthBridge.op0_busGain
                        onCarrierLevelEdited: (v) => SynthBridge.op0_carrierLevel = v
                        onAmRateEdited: (v) => SynthBridge.op0_amRate = v
                        onAmDepthEdited: (v) => SynthBridge.op0_amDepth = v
                        onAmEnvAmountEdited: (v) => SynthBridge.op0_amEnvAmount = v
                        onVibratoRateEdited: (v) => SynthBridge.op0_vibratoRate = v
                        onVibratoDepthEdited: (v) => SynthBridge.op0_vibratoDepth = v
                        onModFreqEdited: (v) => SynthBridge.op0_modFreq = v
                        onModIdxEdited: (v) => SynthBridge.op0_modIdx = v
                        onFmEnvAmountEdited: (v) => SynthBridge.op0_fmEnvAmount = v
                        onBusGainEdited: (v) => SynthBridge.op0_busGain = v
                    }

                    OperatorPanel {
                        opIndex: 1
                        carrierLevel: SynthBridge.op1_carrierLevel
                        amRate: SynthBridge.op1_amRate
                        amDepth: SynthBridge.op1_amDepth
                        amEnvAmount: SynthBridge.op1_amEnvAmount
                        vibratoRate: SynthBridge.op1_vibratoRate
                        vibratoDepth: SynthBridge.op1_vibratoDepth
                        modFreq: SynthBridge.op1_modFreq
                        modIdx: SynthBridge.op1_modIdx
                        fmEnvAmount: SynthBridge.op1_fmEnvAmount
                        busGain: SynthBridge.op1_busGain
                        onCarrierLevelEdited: (v) => SynthBridge.op1_carrierLevel = v
                        onAmRateEdited: (v) => SynthBridge.op1_amRate = v
                        onAmDepthEdited: (v) => SynthBridge.op1_amDepth = v
                        onAmEnvAmountEdited: (v) => SynthBridge.op1_amEnvAmount = v
                        onVibratoRateEdited: (v) => SynthBridge.op1_vibratoRate = v
                        onVibratoDepthEdited: (v) => SynthBridge.op1_vibratoDepth = v
                        onModFreqEdited: (v) => SynthBridge.op1_modFreq = v
                        onModIdxEdited: (v) => SynthBridge.op1_modIdx = v
                        onFmEnvAmountEdited: (v) => SynthBridge.op1_fmEnvAmount = v
                        onBusGainEdited: (v) => SynthBridge.op1_busGain = v
                    }

                    OperatorPanel {
                        opIndex: 2
                        carrierLevel: SynthBridge.op2_carrierLevel
                        amRate: SynthBridge.op2_amRate
                        amDepth: SynthBridge.op2_amDepth
                        amEnvAmount: SynthBridge.op2_amEnvAmount
                        vibratoRate: SynthBridge.op2_vibratoRate
                        vibratoDepth: SynthBridge.op2_vibratoDepth
                        modFreq: SynthBridge.op2_modFreq
                        modIdx: SynthBridge.op2_modIdx
                        fmEnvAmount: SynthBridge.op2_fmEnvAmount
                        busGain: SynthBridge.op2_busGain
                        onCarrierLevelEdited: (v) => SynthBridge.op2_carrierLevel = v
                        onAmRateEdited: (v) => SynthBridge.op2_amRate = v
                        onAmDepthEdited: (v) => SynthBridge.op2_amDepth = v
                        onAmEnvAmountEdited: (v) => SynthBridge.op2_amEnvAmount = v
                        onVibratoRateEdited: (v) => SynthBridge.op2_vibratoRate = v
                        onVibratoDepthEdited: (v) => SynthBridge.op2_vibratoDepth = v
                        onModFreqEdited: (v) => SynthBridge.op2_modFreq = v
                        onModIdxEdited: (v) => SynthBridge.op2_modIdx = v
                        onFmEnvAmountEdited: (v) => SynthBridge.op2_fmEnvAmount = v
                        onBusGainEdited: (v) => SynthBridge.op2_busGain = v
                    }

                    OperatorPanel {
                        opIndex: 3
                        carrierLevel: SynthBridge.op3_carrierLevel
                        amRate: SynthBridge.op3_amRate
                        amDepth: SynthBridge.op3_amDepth
                        amEnvAmount: SynthBridge.op3_amEnvAmount
                        vibratoRate: SynthBridge.op3_vibratoRate
                        vibratoDepth: SynthBridge.op3_vibratoDepth
                        modFreq: SynthBridge.op3_modFreq
                        modIdx: SynthBridge.op3_modIdx
                        fmEnvAmount: SynthBridge.op3_fmEnvAmount
                        busGain: SynthBridge.op3_busGain
                        onCarrierLevelEdited: (v) => SynthBridge.op3_carrierLevel = v
                        onAmRateEdited: (v) => SynthBridge.op3_amRate = v
                        onAmDepthEdited: (v) => SynthBridge.op3_amDepth = v
                        onAmEnvAmountEdited: (v) => SynthBridge.op3_amEnvAmount = v
                        onVibratoRateEdited: (v) => SynthBridge.op3_vibratoRate = v
                        onVibratoDepthEdited: (v) => SynthBridge.op3_vibratoDepth = v
                        onModFreqEdited: (v) => SynthBridge.op3_modFreq = v
                        onModIdxEdited: (v) => SynthBridge.op3_modIdx = v
                        onFmEnvAmountEdited: (v) => SynthBridge.op3_fmEnvAmount = v
                        onBusGainEdited: (v) => SynthBridge.op3_busGain = v
                    }

                    OperatorPanel {
                        opIndex: 4
                        carrierLevel: SynthBridge.op4_carrierLevel
                        amRate: SynthBridge.op4_amRate
                        amDepth: SynthBridge.op4_amDepth
                        amEnvAmount: SynthBridge.op4_amEnvAmount
                        vibratoRate: SynthBridge.op4_vibratoRate
                        vibratoDepth: SynthBridge.op4_vibratoDepth
                        modFreq: SynthBridge.op4_modFreq
                        modIdx: SynthBridge.op4_modIdx
                        fmEnvAmount: SynthBridge.op4_fmEnvAmount
                        busGain: SynthBridge.op4_busGain
                        onCarrierLevelEdited: (v) => SynthBridge.op4_carrierLevel = v
                        onAmRateEdited: (v) => SynthBridge.op4_amRate = v
                        onAmDepthEdited: (v) => SynthBridge.op4_amDepth = v
                        onAmEnvAmountEdited: (v) => SynthBridge.op4_amEnvAmount = v
                        onVibratoRateEdited: (v) => SynthBridge.op4_vibratoRate = v
                        onVibratoDepthEdited: (v) => SynthBridge.op4_vibratoDepth = v
                        onModFreqEdited: (v) => SynthBridge.op4_modFreq = v
                        onModIdxEdited: (v) => SynthBridge.op4_modIdx = v
                        onFmEnvAmountEdited: (v) => SynthBridge.op4_fmEnvAmount = v
                        onBusGainEdited: (v) => SynthBridge.op4_busGain = v
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
