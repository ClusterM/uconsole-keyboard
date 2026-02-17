cmake_minimum_required(VERSION 3.16)
project(plasma-uconsole-keyboard LANGUAGES CXX)

find_package(ECM 1.4.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

set(QT_MIN_VERSION "5.15.0")
find_package(Qt6 ${QT_MIN_VERSION} QUIET COMPONENTS Core Qml)
if (Qt6_FOUND)
    set(QT_MAJOR_VERSION 6)
    find_package(KF6 REQUIRED COMPONENTS Plasma)
else()
    find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS Core Qml)
    set(QT_MAJOR_VERSION 5)
    find_package(KF5 REQUIRED COMPONENTS Plasma)
endif()

# Install the plasmoid package (metadata + QML)
plasma_install_package(${CMAKE_CURRENT_SOURCE_DIR} org.kde.plasma.uconsole-keyboard)

# QML extension plugin (KeyboardState type)
set(uconsolekeyboardplugin_SRCS
    plugin/keyboardstate.cpp
    plugin/uconsolekeyboardplugin.cpp
)
add_library(uconsolekeyboardplugin SHARED ${uconsolekeyboardplugin_SRCS})
target_link_libraries(uconsolekeyboardplugin
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::Qml
)
target_include_directories(uconsolekeyboardplugin PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/plugin)
install(TARGETS uconsolekeyboardplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/private/uconsolekeyboard)
install(FILES plugin/qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/private/uconsolekeyboard)
