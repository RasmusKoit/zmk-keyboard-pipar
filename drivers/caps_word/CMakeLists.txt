zephyr_include_directories(${ZEPHYR_CURRENT_MODULE_DIR}/include)
zephyr_include_directories(${APPLICATION_SOURCE_DIR}/include)
zephyr_library_sources(caps_word_state_changed.c)
set_source_files_properties(
        ${APPLICATION_SOURCE_DIR}/src/behaviors/behavior_caps_word.c
        TARGET_DIRECTORY app
        PROPERTIES HEADER_FILE_ONLY ON)
target_sources(app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/behavior_caps_word.c)
