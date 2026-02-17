import QtQuick
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.plasma.components as PlasmaComponents
import org.kde.plasma.plasmoid

PlasmaComponents.FormLayout {
    Layout.fillWidth: true

    PlasmaComponents.TextField {
        id: scriptsPathField
        Layout.fillWidth: true
        label: i18n("Scripts directory:")
        placeholderText: i18n("Leave empty for default (parent of plasmoid)")
        text: plasmoid.configuration.scriptsPath || ""
        onTextChanged: plasmoid.configuration.scriptsPath = text
    }
}
