cmake_minimum_required(VERSION 3.20)

project(Promptery VERSION 0.2 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

add_custom_target(doc SOURCES
    README.md
    .clang-format
    .gitignore
)

add_subdirectory(common)
add_subdirectory(model)
add_subdirectory(ui)
add_subdirectory(app)
