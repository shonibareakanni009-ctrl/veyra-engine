# Veyra Engine — Browser Prototype 001
#
# This is an intentionally small, standalone prototype.
# It proves the first build path without pretending that the full engine,
# editor, or final renderer has already been implemented.
#
# The target can be compiled for a desktop host or for the browser with
# Emscripten. For a browser build, configure through the emcmake wrapper.

cmake_minimum_required(VERSION 3.28)

project(
    VeyraBrowserPrototype
    VERSION 0.1.0
    DESCRIPTION "The first SDL3 and WebAssembly prototype for Veyra Engine"
    LANGUAGES CXX
)

# Require the language version selected for the engine core.
# Extensions are disabled so the prototype stays within standard C++20.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Keep generated executables and browser files in one predictable folder.
# An Emscripten build will place the HTML, JavaScript, and WebAssembly files
# together here so that a simple static web server can serve them.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

include(FetchContent)

# Build SDL3 as a pinned source dependency instead of silently using whatever
# SDL version happens to be installed on the machine.
#
# The URL points to the SDL 3.4.18 source release. The SHA-256 check makes
# CMake reject a downloaded archive whose contents do not match the release
# archive we selected.
#
# SDL_Renderer is used only for this first bootstrap test. The selected
# long-term rendering backend for Veyra remains bgfx.
set(SDL_SHARED OFF CACHE BOOL "Build SDL3 as a shared library" FORCE)
set(SDL_STATIC ON CACHE BOOL "Build SDL3 as a static library" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "Do not build SDL3's optional test library" FORCE)
set(SDL_TESTS OFF CACHE BOOL "Do not build SDL3's optional test programs" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "Do not build SDL3's examples" FORCE)
set(SDL_DISABLE_INSTALL ON CACHE BOOL "Do not create SDL3 install targets" FORCE)

FetchContent_Declare(
    SDL3
    URL https://github.com/libsdl-org/SDL/releases/download/release-3.4.18/SDL3-3.4.18.tar.gz
    URL_HASH SHA256=9c75cf16330322c217dedd2e0609f1124f1b54b8633e763467b4684d0f4334a3
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# Download and configure SDL3 as part of this prototype's build.
FetchContent_MakeAvailable(SDL3)

add_executable(
    veyra_browser_prototype
    src/main.cpp
)

target_link_libraries(
    veyra_browser_prototype
    PRIVATE
        SDL3::SDL3
)

target_compile_features(
    veyra_browser_prototype
    PRIVATE
        cxx_std_20
)

# Use useful compiler warnings without requiring one specific compiler.
if(MSVC)
    target_compile_options(
        veyra_browser_prototype
        PRIVATE
            /W4
            /permissive-
    )
else()
    target_compile_options(
        veyra_browser_prototype
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )
endif()

if(EMSCRIPTEN)
    # Emscripten normally emits a JavaScript launcher and a .wasm file.
    # Asking for the .html suffix also generates a ready-to-serve HTML page.
    set_target_properties(
        veyra_browser_prototype
        PROPERTIES
            SUFFIX ".html"
    )

    # The first delivery is browser-only: do not generate Node.js or shell
    # launch paths. Allow the WebAssembly memory to grow if the prototype
    # needs more memory than its initial allocation.
    target_link_options(
        veyra_browser_prototype
        PRIVATE
            "-sENVIRONMENT=web"
            "-sALLOW_MEMORY_GROWTH=1"
            "-sASSERTIONS=1"
    )
endif()
