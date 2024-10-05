set(LIB_SOURCES
    include/model/backend_manager.h
    include/model/chat_types.h
    include/model/defines.h
    include/model/internal_llm_backend.h
    include/model/llm_interface.h
    include/model/ollama_config.h
    include/model/ollama_interface.h
    include/model/workflow_basic.h
    include/model/workflow_processor.h

    src/backend_manager.cpp
    src/internal_llm_backend.cpp
    src/ollama_config.cpp
    src/ollama_interface.cpp
    src/workflow_basic.cpp
    src/workflow_processor.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_library(model STATIC ${LIB_SOURCES})
else()
    add_library(model STATIC ${LIB_SOURCES})
endif()

target_link_libraries(model
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Network
        common
)

target_include_directories(model PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
