# Veyra Runtime — standalone game execution target.
# Keep runtime dependencies separate from the Qt-based editor.
cmake_minimum_required(VERSION 3.28)

project(VeyraRuntime VERSION 0.2.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
include(CTest)

# SDL3 3.4.18 — immutable official release archive, verified by SHA-256.
set(SDL_SHARED OFF CACHE BOOL "Build SDL3 as a shared library" FORCE)
set(SDL_STATIC ON CACHE BOOL "Build SDL3 as a static library" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "Do not build SDL3 test library" FORCE)
set(SDL_TESTS OFF CACHE BOOL "Do not build SDL3 tests" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "Do not build SDL3 examples" FORCE)
set(SDL_INSTALL OFF CACHE BOOL "Do not install SDL3 separately" FORCE)

FetchContent_Declare(
    SDL3
    URL https://github.com/libsdl-org/SDL/releases/download/release-3.4.18/SDL3-3.4.18.tar.gz
    URL_HASH SHA256=9c75cf16330322c217dedd2e0609f1124f1b54b8633e763467b4684d0f4334a3
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# nlohmann/json 3.12.0 — header-only parser, pinned and hash-verified.
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")
FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
    URL_HASH SHA256=42f6e95cad6ec532fd372391373363b62a14af6d771056dbfc86160e6dfff7aa
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(SDL3 nlohmann_json)

# bgfx.cmake — pinned to an immutable upstream commit with recursive submodules.
# Use two precompiled DXBC shaders pinned from the same bgfx submodule revision.
# This keeps normal Windows builds independent of the heavyweight shader-toolchain.
set(BGFX_BUILD_TOOLS OFF CACHE BOOL "Do not build bgfx tools in normal runtime builds" FORCE)
set(BGFX_BUILD_TOOLS_BIN2C OFF CACHE BOOL "Do not build standalone bin2c" FORCE)
set(BGFX_BUILD_TOOLS_SHADER OFF CACHE BOOL "Do not build bgfx shaderc in normal runtime builds" FORCE)
set(BGFX_BUILD_TOOLS_GEOMETRY OFF CACHE BOOL "Do not build geometry tools" FORCE)
set(BGFX_BUILD_TOOLS_TEXTURE OFF CACHE BOOL "Do not build texture tools" FORCE)
set(BGFX_BUILD_EXAMPLES OFF CACHE BOOL "Do not build bgfx examples" FORCE)
set(BGFX_BUILD_TESTS OFF CACHE BOOL "Do not build bgfx tests" FORCE)
set(BGFX_INSTALL OFF CACHE BOOL "Do not install bgfx separately" FORCE)
set(BGFX_CUSTOM_TARGETS OFF CACHE BOOL "Disable bgfx convenience targets" FORCE)
set(BGFX_LIBRARY_TYPE STATIC CACHE STRING "Link bgfx statically" FORCE)
set(BGFX_CONFIG_VIDEO OFF CACHE BOOL "Disable optional video decoding" FORCE)

FetchContent_Declare(
    bgfx_cmake
    GIT_REPOSITORY https://github.com/bkaradzic/bgfx.cmake.git
    GIT_TAG eb88f6be55ae223966698c802542c24b0d93fd25
    GIT_SHALLOW FALSE
    GIT_SUBMODULES_RECURSE TRUE
)
FetchContent_MakeAvailable(bgfx_cmake)

add_library(VeyraRuntimeCore
    src/scene_document.cpp
    src/scene_document.h
    src/scene_geometry.cpp
    src/scene_geometry.h
)
target_link_libraries(VeyraRuntimeCore PRIVATE nlohmann_json::nlohmann_json)
target_include_directories(VeyraRuntimeCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_features(VeyraRuntimeCore PUBLIC cxx_std_20)

add_executable(VeyraRuntime
    src/main.cpp
    src/runtime_renderer.cpp
    src/runtime_renderer.h
    src/embedded_shaders.h
)
target_link_libraries(VeyraRuntime PRIVATE VeyraRuntimeCore SDL3::SDL3-static bgfx)
target_compile_features(VeyraRuntime PRIVATE cxx_std_20)

function(veyra_enable_warnings target_name)
    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

veyra_enable_warnings(VeyraRuntimeCore)
veyra_enable_warnings(VeyraRuntime)

if(BUILD_TESTING)
    add_executable(VeyraRuntimeTests
        tests/scene_document_tests.cpp
    )
    target_link_libraries(VeyraRuntimeTests PRIVATE VeyraRuntimeCore)
    target_compile_features(VeyraRuntimeTests PRIVATE cxx_std_20)
    veyra_enable_warnings(VeyraRuntimeTests)
    add_test(NAME VeyraRuntimeSceneTests COMMAND VeyraRuntimeTests)

    add_executable(VeyraRuntimeGeometryTests
        tests/scene_geometry_tests.cpp
    )
    target_link_libraries(VeyraRuntimeGeometryTests PRIVATE VeyraRuntimeCore)
    target_compile_features(VeyraRuntimeGeometryTests PRIVATE cxx_std_20)
    veyra_enable_warnings(VeyraRuntimeGeometryTests)
    add_test(NAME VeyraRuntimeGeometryTests COMMAND VeyraRuntimeGeometryTests)
endif()

install(TARGETS VeyraRuntime RUNTIME DESTINATION .)
install(FILES "${sdl3_SOURCE_DIR}/LICENSE.txt" DESTINATION licenses RENAME SDL3-LICENSE.txt)
install(FILES "${nlohmann_json_SOURCE_DIR}/LICENSE.MIT" DESTINATION licenses RENAME nlohmann-json-LICENSE.MIT)
install(FILES "${bgfx_cmake_SOURCE_DIR}/bgfx/LICENSE" DESTINATION licenses RENAME bgfx-LICENSE)
install(FILES "${bgfx_cmake_SOURCE_DIR}/bx/LICENSE" DESTINATION licenses RENAME bx-LICENSE)
install(FILES "${bgfx_cmake_SOURCE_DIR}/bimg/LICENSE" DESTINATION licenses RENAME bimg-LICENSE)
