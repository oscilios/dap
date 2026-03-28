import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Sub37

RowLayout {
    spacing: 2

    function noteName(midi) {
        if (midi < 0) return "-";
        var names = ["C","C#","D","D#","E","F","F#","G","G#","A","A#","B"];
        return names[midi % 12] + (Math.floor(midi / 12) - 1);
    }

    Repeater {
        model: 32

        Rectangle {
            required property int index
            readonly property int stepNote: {
                var notes = SynthBridge.seqNotes;
                return index < notes.length ? notes[index] : -1;
            }
            readonly property bool inRange: index < SynthBridge.seqStepCount
            readonly property bool isPlaying: SynthBridge.seqCurrentStep === index
            readonly property bool isSelected: SynthBridge.seqSelectedStep === index
            readonly property bool hasNote: stepNote >= 0

            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: 2
            opacity: inRange ? 1.0 : 0.3
            color: isPlaying ? "#d4a017"
                 : isSelected ? "#1a5276"
                 : hasNote ? "#2a2a2a"
                 : "#1a1a1a"
            border.color: isSelected ? "#4fc3f7"
                        : isPlaying ? "#f0c040"
                        : "#333333"
            border.width: isSelected || isPlaying ? 2 : 1

            Label {
                anchors.centerIn: parent
                text: hasNote ? noteName(stepNote) : (index + 1).toString()
                font.pixelSize: hasNote ? 9 : 7
                font.weight: hasNote ? Font.Medium : Font.Normal
                color: isPlaying ? "#1a1a1a"
                     : hasNote ? "#e0e0e0"
                     : "#555555"
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (SynthBridge.seqSelectedStep === index) {
                        if (hasNote) {
                            SynthBridge.seqClearStep(index);
                        } else {
                            SynthBridge.seqSelectedStep = -1;
                        }
                    } else {
                        SynthBridge.seqSelectedStep = index;
                    }
                }
            }
        }
    }
}
