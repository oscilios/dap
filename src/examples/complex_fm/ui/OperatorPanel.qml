import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ScrollView {
    id: root

    property int opIndex: 0

    property real carrierLevel: 0.7
    property real amRate: 0.8
    property real amDepth: 0.15
    property real amEnvAmount: 0.0
    property real vibratoRate: 5.8
    property real vibratoDepth: 0.2
    property real modFreq: 196.0
    property real modIdx: 3.0
    property real fmEnvAmount: 0.0
    property real busGain: 0.7

    signal carrierLevelEdited(real v)
    signal amRateEdited(real v)
    signal amDepthEdited(real v)
    signal amEnvAmountEdited(real v)
    signal vibratoRateEdited(real v)
    signal vibratoDepthEdited(real v)
    signal modFreqEdited(real v)
    signal modIdxEdited(real v)
    signal fmEnvAmountEdited(real v)
    signal busGainEdited(real v)

    contentWidth: availableWidth

    ColumnLayout {
        width: root.availableWidth
        spacing: 16

        // FM (carrier + modulator)
        GroupBox {
            title: "FM"
            Layout.fillWidth: true
            RowLayout {
                spacing: 12
                RotarySlider {
                    label: "Level"
                    from: 0; to: 1; value: root.carrierLevel
                    onValueChanged: root.carrierLevelEdited(value)
                }
                RotarySlider {
                    label: "Bus Gain"
                    from: 0; to: 1; value: root.busGain
                    onValueChanged: root.busGainEdited(value)
                }
                RotarySlider {
                    label: "Mod Freq"
                    from: 0; to: 2000; value: root.modFreq
                    stepSize: 1; decimals: 0
                    onValueChanged: root.modFreqEdited(value)
                }
                RotarySlider {
                    label: "Mod Index"
                    from: 0; to: 20; value: root.modIdx
                    stepSize: 0.1; decimals: 1
                    onValueChanged: root.modIdxEdited(value)
                }
                RotarySlider {
                    label: "Env Amt"
                    from: 0; to: 5000; value: root.fmEnvAmount
                    stepSize: 10; decimals: 0
                    onValueChanged: root.fmEnvAmountEdited(value)
                }
            }
        }

        // AM
        GroupBox {
            title: "Amplitude Modulation"
            Layout.fillWidth: true
            RowLayout {
                spacing: 12
                RotarySlider {
                    label: "Rate"
                    from: 0; to: 20; value: root.amRate
                    stepSize: 0.1; decimals: 1
                    onValueChanged: root.amRateEdited(value)
                }
                RotarySlider {
                    label: "Depth"
                    from: 0; to: 1; value: root.amDepth
                    onValueChanged: root.amDepthEdited(value)
                }
                RotarySlider {
                    label: "Env Amt"
                    from: 0; to: 1; value: root.amEnvAmount
                    onValueChanged: root.amEnvAmountEdited(value)
                }
            }
        }

        // Vibrato
        GroupBox {
            title: "Vibrato"
            Layout.fillWidth: true
            RowLayout {
                spacing: 12
                RotarySlider {
                    label: "Rate"
                    from: 0; to: 20; value: root.vibratoRate
                    stepSize: 0.1; decimals: 1
                    onValueChanged: root.vibratoRateEdited(value)
                }
                RotarySlider {
                    label: "Depth"
                    from: 0; to: 2; value: root.vibratoDepth
                    onValueChanged: root.vibratoDepthEdited(value)
                }
            }
        }
    }
}
