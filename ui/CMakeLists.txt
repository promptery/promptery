set(LIB_SOURCES
    include/ui/combobox.h
    include/ui/pushbutton.h
    include/ui/treeview.h

    include/ui/chat_input_widget.h
    include/ui/chat_item_widget.h
    include/ui/chat_main_widget.h
    include/ui/chat_model.h
    include/ui/chat_output_widget.h
    include/ui/chat_request_config_model.h
    include/ui/chat_request_config_widget.h
    include/ui/chat_widget.h
    include/ui/content_model.h
    include/ui/content_widget.h
    include/ui/decorator_prompt_model.h
    include/ui/decorator_prompt_widget.h
    include/ui/defines.h
    include/ui/enhanced_treemodel.h
    include/ui/enhanced_treeview.h
    include/ui/file_system_model.h
    include/ui/label.h
    include/ui/log_widget.h
    include/ui/model_interface.h
    include/ui/named_object_model.h
    include/ui/named_object_view.h
    include/ui/page_interface.h
    include/ui/request_config_widget.h
    include/ui/settings_widget.h
    include/ui/system_prompt_model.h
    include/ui/system_prompt_widget.h
    include/ui/tab_view.h
    include/ui/tile_child_interface.h
    include/ui/tile_widget.h
    include/ui/workflow_adapter.h
    include/ui/workflow_basic.h
    include/ui/workflow_model.h
    include/ui/workflow_widget.h

    src/chat_input_widget.cpp
    src/chat_item_widget.cpp
    src/chat_main_widget.cpp
    src/chat_main_widget.ui
    src/chat_output_widget.cpp
    src/chat_request_config_model.cpp
    src/chat_request_config_widget.cpp
    src/chat_widget.cpp
    src/chat_widget.ui
    src/content_widget.cpp
    src/content_widget.ui
    src/decorator_prompt_widget.cpp
    src/decorator_prompt_widget.ui
    src/defines.cpp
    src/enhanced_treemodel.cpp
    src/enhanced_treeview.cpp
    src/file_system_model.cpp
    src/log_widget.cpp
    src/log_widget.ui
    src/named_object_model.cpp
    src/named_object_view.cpp
    src/settings_widget.cpp
    src/system_prompt_widget.cpp
    src/system_prompt_widget.ui
    src/tab_view.cpp
    src/tile_widget.cpp
    src/workflow_basic.cpp
    src/workflow_model.cpp
    src/workflow_widget.cpp

    res/ui.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_library(ui STATIC ${LIB_SOURCES})
else()
    add_library(ui STATIC ${LIB_SOURCES})
endif()

target_link_libraries(ui
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Widgets
        model
)

target_include_directories(ui PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
