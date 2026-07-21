# Controls whether a given observe executable target links as a native
# Windows GUI application (/SUBSYSTEM:WINDOWS, no console window) or a
# console application (/SUBSYSTEM:CONSOLE, stdout/stderr visible).
#
# src/app/main.cpp defines both WinMain() and main() unconditionally on
# Windows. The WIN32_EXECUTABLE target property set below controls which
# CRT startup stub - and therefore which of the two entry points - actually
# gets used at link time, so no corresponding preprocessor define is needed
# in application code purely for entry-point selection. observe and
# observe-console are both built from this same main.cpp, each just ends up
# with a different value here. If application *behavior* ever needs to
# differ between the two (e.g. attaching a console only in console mode), a
# compile definition can be added alongside this call at that point.

function(observe_set_windows_subsystem TARGET GUI)

    if(NOT WIN32)
        return()
    endif()

    if(GUI)
        set_target_properties(${TARGET}
            PROPERTIES
                WIN32_EXECUTABLE TRUE
        )
    else()
        set_target_properties(${TARGET}
            PROPERTIES
                WIN32_EXECUTABLE FALSE
        )
    endif()

endfunction()