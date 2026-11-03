# Veyra Editor — first native desktop application.
# This host tool is separate from exported game runtimes; games must not need Qt.
cmake_minimum_required(VERSION 3.28)
project(VeyraEditor VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 6.8 REQUIRED COMPONENTS Widgets Test)
include(CTest)

qt_add_executable(VeyraEditor WIN32
    src/main.cpp
    src/editor_main_window.cpp
    src/editor_main_window.h
    src/scene_viewport.cpp
    src/scene_viewport.h
    src/scene_document.cpp
    src/scene_document.h
)
target_link_libraries(VeyraEditor PRIVATE Qt6::Widgets)
target_compile_features(VeyraEditor PRIVATE cxx_std_20)

if(MSVC)
    target_compile_options(VeyraEditor PRIVATE /W4 /permissive-)
else()
    target_compile_options(VeyraEditor PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Keep the executable in package/ so deployment can stage runtime files beside it.
set_target_properties(VeyraEditor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/package"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/package"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/package"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/package"
)

if(BUILD_TESTING)
    qt_add_executable(VeyraEditorTests
        tests/scene_document_tests.cpp
        src/scene_document.cpp
        src/scene_document.h
    )
    target_link_libraries(VeyraEditorTests PRIVATE Qt6::Test)
    target_compile_features(VeyraEditorTests PRIVATE cxx_std_20)
    if(MSVC)
        target_compile_options(VeyraEditorTests PRIVATE /W4 /permissive-)
    else()
        target_compile_options(VeyraEditorTests PRIVATE -Wall -Wextra -Wpedantic)
    endif()
    add_test(NAME VeyraSceneDocumentTests COMMAND VeyraEditorTests)
endif()
