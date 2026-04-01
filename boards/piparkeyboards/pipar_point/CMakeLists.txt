if(BOARD_REVISION STREQUAL "right")
    message(STATUS "Checking for required snippets for BOARD_REVISION: ${BOARD_REVISION}")

    if(SNIPPET MATCHES ";")
        set (SNIPPET_LIST ${SNIPPET})
    else()
    # Convert space-separated string into a proper CMake list
        string(REPLACE " " ";" SNIPPET_LIST "${SNIPPET}")
    endif()
    message(STATUS "SNIPPET_LIST elements: ${SNIPPET_LIST}")
    if(NOT "trackpoint" IN_LIST SNIPPET_LIST)
        message(FATAL_ERROR "trackpoint snippet must be included to build this board")
    endif()
endif()