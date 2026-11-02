# Veyra Editor — first native desktop application.
# This host tool is separate from exported game runtimes; games must not need Qt.
cmake_minimum_required(VERSION 3.28)
project(VeyraEditor VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
find_package(Qt6 6.8 REQUIRED COMPONENTS Widgets)
qt_add_executable(VeyraEditor WIN32
    src/main.cpp
    src/editor_main_window.cpp
    src/editor_main_window.h
    src/scene_viewport.cpp
    src/scene_viewport.h
)
target_link_libraries(VeyraEditor PRIVATE Qt6::Widgets)
target_compile_features(VeyraEditor PRIVATE cxx_std_20)
if(MSVC)
    target_compile_options(VeyraEditor PRIVATE /W4 /permissive-)
else()
    target_compile_options(VeyraEditor PRIVATE -Wall -Wextra -Wpedantic)
endif()
# The package folder receives the EXE first; windeployqt adds Qt DLLs/plugins.
set_target_properties(VeyraEditor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "\${CMAKE_BINARY_DIR}/package"
)
