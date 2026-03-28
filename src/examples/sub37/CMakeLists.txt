set (target sub37_synth)
set (sources
    AudioProcess.cpp
    main.cpp
    Sub37Synth.cpp
    )

find_package(Qt6 COMPONENTS Quick QuickControls2 Qml)

if (Qt6_FOUND)
    set(CMAKE_AUTOMOC ON)
    qt_add_executable (${target} ${sources} ui/SynthBridge.cpp)

    qt_add_qml_module(${target}
        URI Sub37
        VERSION 1.0
        QML_FILES
            ui/Main.qml
            ui/OscillatorPanel.qml
            ui/FilterPanel.qml
            ui/EnvelopePanel.qml
            ui/ModulationPanel.qml
            ui/ArpPanel.qml
            ui/PianoKeyboard.qml
            ui/RotarySlider.qml
            ui/SmallRadioButton.qml
        SOURCES
            ui/SynthBridge.h
            ui/SynthBridge.cpp
    )

    target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ui)
    target_link_libraries (${target} PRIVATE
        dap_crtp_utility dap_osc dap_audioio
        Qt6::Quick Qt6::QuickControls2 Qt6::Qml)
    target_compile_definitions(${target} PRIVATE DAP_HAS_QT=1)
else()
    add_executable (${target} ${sources})
    target_link_libraries (${target} dap_crtp_utility dap_osc dap_audioio)
endif()
