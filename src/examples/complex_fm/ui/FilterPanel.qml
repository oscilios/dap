import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ComplexFM

ScrollView {
    contentWidth: availableWidth

    ColumnLayout {
        width: parent.width
        spacing: 16

        // Ladder Filter
        GroupBox {
            title: "Ladder Filter"
            Layout.fillWidth: true
            RowLayout {
                spacing: 12
                RotarySlider {
                    label: "Resonance"
                    from: 0; to: 1; value: SynthBridge.filterResonance
                    onValueChanged: SynthBridge.filterResonance = value
                }
            }
        }

        // EQ
        GroupBox {
            title: "Biquad EQ"
            Layout.fillWidth: true
            ColumnLayout {
                spacing: 8
                RowLayout {
                    spacing: 12
                    RotarySlider {
                        label: "Frequency"
                        from: 20; to: 20000; value: SynthBridge.eqFrequency
                        stepSize: 10; decimals: 0
                        onValueChanged: SynthBridge.eqFrequency = value
                    }
                    RotarySlider {
                        label: "Q"
                        from: 0.1; to: 10; value: SynthBridge.eqQ
                        stepSize: 0.01; decimals: 2
                        onValueChanged: SynthBridge.eqQ = value
                    }
                    RotarySlider {
                        label: "Gain (dB)"
                        from: -24; to: 24; value: SynthBridge.eqGainDb
                        stepSize: 0.5; decimals: 1
                        onValueChanged: SynthBridge.eqGainDb = value
                    }
                }
                RowLayout {
                    spacing: 8
                    Label { text: "Type:"; color: "#cccccc"; font.pixelSize: 12 }
                    Repeater {
                        model: ["LowPass", "HighPass", "BandPass", "Notch", "Peak", "LowShelf", "HighShelf"]
                        RadioButton {
                            text: modelData
                            checked: SynthBridge.eqType === index
                            onClicked: SynthBridge.eqType = index
                            contentItem: Label {
                                text: parent.text
                                color: "#cccccc"
                                font.pixelSize: 11
                                leftPadding: parent.indicator.width + 4
                            }
                        }
                    }
                }
            }
        }
    }
}
