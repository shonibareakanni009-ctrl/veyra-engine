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

add_library(VeyraRuntimeCore
    src/scene_document.cpp
    src/scene_document.h
)
target_link_libraries(VeyraRuntimeCore PRIVATE nlohmann_json::nlohmann_json)
target_compile_features(VeyraRuntimeCore PUBLIC cxx_std_20)

add_executable(VeyraRuntime
    src/main.cpp
)
target_link_libraries(VeyraRuntime PRIVATE VeyraRuntimeCore SDL3::SDL3-static)
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
endif()

install(TARGETS VeyraRuntime RUNTIME DESTINATION .)
