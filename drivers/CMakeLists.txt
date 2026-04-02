add_subdirectory_ifdef(CONFIG_PIPAR_CAPS_INDICATOR indicator)

if(CONFIG_DT_HAS_ZMK_BEHAVIOR_CAPS_WORD_ENABLED)
        zephyr_include_directories(${ZEPHYR_CURRENT_MODULE_DIR}/include)
        zephyr_include_directories(${APPLICATION_SOURCE_DIR}/include)
        target_sources(app PRIVATE ${ZEPHYR_CURRENT_MODULE_DIR}/src/events/caps_word_state_changed.c)
        set_source_files_properties(
                ${APPLICATION_SOURCE_DIR}/src/behaviors/behavior_caps_word.c
                TARGET_DIRECTORY app
                PROPERTIES HEADER_FILE_ONLY ON)
        target_sources(app PRIVATE ${ZEPHYR_CURRENT_MODULE_DIR}/src/behaviors/behavior_caps_word.c)
endif()
