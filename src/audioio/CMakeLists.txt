set (target_name dap_audioio)
set (headers
     IAudioProcess.h
     AudioBusFactory.h
     IAudioBus.h
     IAudioDevice.h
     AudioDeviceList.h
     Scope.h
    )
set (sources
     NullAudioProcess.cpp
     AudioBusFactory.cpp
     AudioDeviceList.cpp
    )

if (APPLE)
    #add_subdirectory(coreaudio)
    list (APPEND headers
                 coreaudio/AudioDevice.h
                 coreaudio/AudioDeviceListener.h
                 coreaudio/AudioUtilities.h
                 coreaudio/AudioInput.h
                 coreaudio/AudioOutput.h
         )
     list (APPEND sources
                  coreaudio/AudioDevice.cpp
                  coreaudio/AudioDeviceListener.cpp
                  coreaudio/AudioUtilities.cpp
                  coreaudio/AudioInput.cpp
                  coreaudio/AudioOutput.cpp
          )
endif ()

add_library (${target_name} ${sources} ${headers})
target_include_directories (${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if (APPLE)
    FIND_LIBRARY (COREAUDIO_LIBRARY CoreAudio)
    FIND_LIBRARY (COREFOUNDATION_LIBRARY CoreFoundation)
    FIND_LIBRARY (AUDIOTOOLBOX_LIBRARY AudioToolbox)
    FIND_LIBRARY (AUDIOUNIT_LIBRARY AudioUnit)
    target_link_libraries (${target_name}
                          dap_fastmath
                          ${COREAUDIO_LIBRARY}
                          ${COREFOUNDATION_LIBRARY}
                          ${AUDIOUNIT_LIBRARY}
                          ${AUDIOTOOLBOX_LIBRARY}
                          )
endif ()

target_link_libraries (${target_name} ${link_libs})
add_subdirectory (test)
