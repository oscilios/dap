import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ComplexFM

Item {
    id: root

    property int octaveOffset: 0  // shift in octaves from base (C3=48)
    readonly property int baseStart: 48
    readonly property int startNote: baseStart + octaveOffset * 12
    readonly property int endNote: startNote + 24  // always 2 octaves
    readonly property int whiteKeyCount: {
        var count = 0;
        for (var n = startNote; n <= endNote; ++n) {
            if (!isBlackKey(n)) count++;
        }
        return count;
    }
    readonly property real whiteKeyWidth: whiteKeyCount > 0
        ? (keysArea.width - (whiteKeyCount - 1)) / whiteKeyCount : 0

    property int activeNote: -1

    implicitHeight: 120

    // Computer keyboard mapping — offsets relative to current startNote
    readonly property var keyOffsets: ({
        "a": 0, "w": 1, "s": 2, "e": 3, "d": 4, "f": 5,
        "t": 6, "g": 7, "y": 8, "h": 9, "u": 10, "j": 11,
        "k": 12, "o": 13, "l": 14, "p": 15, ";": 16
    })

    focus: true
    Keys.onPressed: function(event) {
        if (event.isAutoRepeat) return;
        if (event.key === Qt.Key_Left) {
            if (startNote > 12) octaveOffset--;
            event.accepted = true;
            return;
        }
        if (event.key === Qt.Key_Right) {
            if (endNote < 120) octaveOffset++;
            event.accepted = true;
            return;
        }
        var offset = keyOffsets[event.text];
        if (offset !== undefined) {
            var note = startNote + offset;
            if (note <= 127) {
                activeNote = note;
                SynthBridge.noteOn(note);
            }
            event.accepted = true;
        }
    }
    Keys.onReleased: function(event) {
        if (event.isAutoRepeat) return;
        var offset = keyOffsets[event.text];
        if (offset !== undefined) {
            var note = startNote + offset;
            if (activeNote === note) {
                activeNote = -1;
                SynthBridge.noteOff();
            }
            event.accepted = true;
        }
    }

    function isBlackKey(note) {
        var pc = note % 12;
        return pc === 1 || pc === 3 || pc === 6 || pc === 8 || pc === 10;
    }

    function whiteKeyIndex(note) {
        var idx = 0;
        for (var n = startNote; n < note; ++n) {
            if (!isBlackKey(n)) idx++;
        }
        return idx;
    }

    function noteName(midi) {
        var names = ["C","C#","D","D#","E","F","F#","G","G#","A","A#","B"];
        return names[midi % 12] + (Math.floor(midi / 12) - 1);
    }

    // Octave navigation
    RowLayout {
        id: octaveBar
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: 20
        spacing: 4

        Button {
            text: "\u25C0"
            font.pixelSize: 12
            implicitWidth: 28
            implicitHeight: 18
            enabled: root.startNote > 12
            onClicked: root.octaveOffset--
        }

        Label {
            text: root.noteName(root.startNote) + " - " + root.noteName(root.endNote)
            font.pixelSize: 11
            color: "#aaaaaa"
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
        }

        Button {
            text: "\u25B6"
            font.pixelSize: 12
            implicitWidth: 28
            implicitHeight: 18
            enabled: root.endNote < 120
            onClicked: root.octaveOffset++
        }
    }

    // Piano keys area
    Item {
        id: keysArea
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: octaveBar.bottom
        anchors.topMargin: 2
        anchors.bottom: parent.bottom

        // White keys
        Row {
            anchors.fill: parent
            spacing: 1

            Repeater {
                model: {
                    var notes = [];
                    for (var n = root.startNote; n <= root.endNote; ++n) {
                        if (!root.isBlackKey(n)) notes.push(n);
                    }
                    return notes;
                }

                Rectangle {
                    required property int modelData
                    width: root.whiteKeyWidth
                    height: keysArea.height
                    color: root.activeNote === modelData ? "#4fc3f7" : "#eeeeee"
                    border.color: "#999999"
                    border.width: 1
                    radius: 2

                    Label {
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 4
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: root.noteName(modelData)
                        font.pixelSize: 9
                        color: "#666666"
                        visible: modelData % 12 === 0
                    }

                    MouseArea {
                        anchors.fill: parent
                        onPressed: {
                            root.activeNote = modelData;
                            SynthBridge.noteOn(modelData);
                            root.forceActiveFocus();
                        }
                        onReleased: {
                            if (root.activeNote === modelData) {
                                root.activeNote = -1;
                                SynthBridge.noteOff();
                            }
                        }
                    }
                }
            }
        }

        // Black keys
        Repeater {
            model: {
                var notes = [];
                for (var n = root.startNote; n <= root.endNote; ++n) {
                    if (root.isBlackKey(n)) notes.push(n);
                }
                return notes;
            }

            Rectangle {
                required property int modelData
                readonly property int wkIdx: root.whiteKeyIndex(modelData)
                x: wkIdx * (root.whiteKeyWidth + 1) + root.whiteKeyWidth * 0.65
                y: 0
                width: root.whiteKeyWidth * 0.7
                height: keysArea.height * 0.6
                color: root.activeNote === modelData ? "#0288d1" : "#333333"
                border.color: "#111111"
                border.width: 1
                radius: 2
                z: 1

                MouseArea {
                    anchors.fill: parent
                    onPressed: {
                        root.activeNote = modelData;
                        SynthBridge.noteOn(modelData);
                        root.forceActiveFocus();
                    }
                    onReleased: {
                        if (root.activeNote === modelData) {
                            root.activeNote = -1;
                            SynthBridge.noteOff();
                        }
                    }
                }
            }
        }
    }
}
