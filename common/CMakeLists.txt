set(LIB_SOURCES
    include/common/downloader.h
    include/common/job.h
    include/common/log.h
    include/common/settings.h
    include/common/tree_to_list_model_adapter.h

    include/tools/finally.h
    include/tools/is_one_of.h
    include/tools/tools.h

    src/downloader.cpp
    src/job.cpp
    src/tree_to_list_model_adapter.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_library(common STATIC ${LIB_SOURCES})
else()
    add_library(common STATIC ${LIB_SOURCES})
endif()

target_link_libraries(common
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Network
)

target_include_directories(common PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
