import QtQuick
import QtQuick.Layouts
import org.kde.plasma.plasmoid
import org.kde.plasma.components as PlasmaComponents
import org.kde.kirigami as Kirigami
import org.kde.plasma.private.uconsolekeyboard 1.0 as UConsole

PlasmoidItem {
    id: root

    UConsole.KeyboardState {
        id: keyboardState
        packageBasePath: Qt.resolvedUrl("../..").toString()
        scriptsPath: (typeof plasmoid !== "undefined" && plasmoid.configuration.scriptsPath) ? plasmoid.configuration.scriptsPath : ""
        Component.onCompleted: keyboardState.startListener()
    }

    Plasmoid.toolTipSubText: {
        if (keyboardState.currentLayer < 0) return i18n("Keyboard not found")
        return i18n("Layer %1 · FN lock %2", keyboardState.currentLayer,
            keyboardState.fnLockOn ? i18n("on") : i18n("off"))
    }

    compactRepresentation: MouseArea {
        id: compactRoot
        Layout.minimumWidth: compactRow.implicitWidth
        Layout.minimumHeight: compactRow.implicitHeight
        hoverEnabled: true
        onClicked: plasmoid.expanded = !plasmoid.expanded

        RowLayout {
            id: compactRow
            anchors.fill: parent
            spacing: 2

            PlasmaComponents.Label {
                text: keyboardState.currentLayer >= 0 ? ("L" + keyboardState.currentLayer) : "—"
                font.pixelSize: Math.max(Plasmoid.iconSizePixels * 0.6, 10)
            }
            PlasmaComponents.Label {
                visible: keyboardState.fnLockOn
                text: "FN"
                font.pixelSize: Math.max(Plasmoid.iconSizePixels * 0.5, 8)
                opacity: 0.9
            }
        }
    }

    fullRepresentation: Item {
        Layout.minimumWidth: popupLayout.implicitWidth
        Layout.minimumHeight: popupLayout.implicitHeight
        Layout.preferredWidth: 220
        Layout.preferredHeight: popupLayout.implicitHeight

        ColumnLayout {
            id: popupLayout
            anchors.fill: parent
            spacing: Kirigami.Units.smallSpacing

            PlasmaComponents.Label {
                text: i18n("Keyboard layer")
                font.bold: true
            }
            PlasmaComponents.ComboBox {
                id: layerCombo
                Layout.fillWidth: true
                model: 15
                displayText: model[currentIndex] !== undefined ? i18n("Layer %1", currentIndex) : ""
                onActivated: function(index) {
                    keyboardState.setLayer(index)
                }
                Component.onCompleted: {
                    currentIndex = keyboardState.currentLayer >= 0 ? keyboardState.currentLayer : 0
                }
                Connections {
                    target: keyboardState
                    function onCurrentLayerChanged() {
                        if (keyboardState.currentLayer >= 0 && keyboardState.currentLayer !== layerCombo.currentIndex) {
                            layerCombo.currentIndex = keyboardState.currentLayer
                        }
                    }
                }
            }

            PlasmaComponents.CheckBox {
                id: fnLockCheck
                Layout.fillWidth: true
                text: i18n("FN lock")
                checked: keyboardState.fnLockOn
                onToggled: keyboardState.setFnLock(checked)
                Connections {
                    target: keyboardState
                    function onFnLockOnChanged() {
                        fnLockCheck.checked = keyboardState.fnLockOn
                    }
                }
            }
        }
    }
}
