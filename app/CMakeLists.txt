project(Promptery VERSION 0.1 LANGUAGES CXX)

set(PROJECT_SOURCES
    src/main_app.cpp
    src/mainwindow_app.cpp
    src/mainwindow_app.h
    src/mainwindow_app.ui
    src/res.qrc
)

set(app_icon_macos "${CMAKE_CURRENT_SOURCE_DIR}/../packaging/Promptery.icns")
set_source_files_properties(${app_icon_macos} PROPERTIES
    MACOSX_PACKAGE_LOCATION "Resources")

set(licenses_macos "${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE")
set_source_files_properties(${licenses_macos} PROPERTIES
    MACOSX_PACKAGE_LOCATION "Licenses")

set(app_icon_resource_windows "${CMAKE_CURRENT_SOURCE_DIR}/../packaging/icon.rc")

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${app_icon_macos}
        ${licenses_macos}
        ${app_icon_resource_windows}
    )
else()
    add_executable(${PROJECT_NAME}
        ${PROJECT_SOURCES}
        ${app_icon_macos}
        ${licenses_macos}
        ${app_icon_resource_windows}
    )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE ui)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.promptery)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_ICON_FILE Promptery.icns
    XCODE_ATTRIBUTE_EXECUTABLE_NAME ${PROJECT_NAME}
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(APPLE)
    string(TOLOWER "${CMAKE_BUILD_TYPE}" build_type_lower)
    if(NOT build_type_lower MATCHES "debug")
        # find macdeployqt - mandatory
        get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
        get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
        find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${_qt_bin_dir}")

        # call macdeployqt
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND "${MACDEPLOYQT_EXECUTABLE}"
            ARGS "Promptery.app"
            COMMENT "Execute macdeployqt to create macOS bundle"
        )

        # find appdmg - optional
        find_program(APPDMG_EXECUTABLE appdmg)
        if("${APPDMG_EXECUTABLE}" STREQUAL "APPDMG_EXECUTABLE-NOTFOUND")
            message(WARNING "appdmg not found, skipping dmg creation")
        else()
            # copy additional files for appdmg
            set(files_to_copy
                "mac_dmg.json"
                "Promptery.icns"
                "mac_dmg_bkg.png"
                "mac_dmg_bkg@2x.png"
            )
            foreach(file ${files_to_copy})
                add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                            "${CMAKE_CURRENT_LIST_DIR}/../packaging/${file}"
                            "${CMAKE_CURRENT_BINARY_DIR}/${file}"
                )
            endforeach()

            add_custom_target(RemoveDmg
                COMMAND ${CMAKE_COMMAND} -E remove -f
                        "${CMAKE_CURRENT_BINARY_DIR}/Promptery.dmg"
                DEPENDS ${PROJECT_NAME}
            )

            # run appdmg and copy result
            add_custom_target(Dmg ALL
                COMMAND "${APPDMG_EXECUTABLE}" "mac_dmg.json" "Promptery.dmg"
                WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
                COMMENT "Package app"
                DEPENDS RemoveDmg
            )
            add_custom_command(TARGET Dmg POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                        "${CMAKE_CURRENT_BINARY_DIR}/Promptery.dmg"
                        "${CMAKE_CURRENT_BINARY_DIR}/../Promptery.dmg"
            )
        endif()
    endif()
endif()

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()
