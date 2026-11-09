cmake_minimum_required(VERSION 3.28)
project(VeyraUpdater VERSION 0.1.0 LANGUAGES C CXX)

if(NOT WIN32)
    message(FATAL_ERROR "The first Veyra updater prototype targets Windows x64 only.")
endif()

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
include(CTest)

# nlohmann/json 3.12.0 — immutable release archive, pinned by SHA-256.
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")
FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
    URL_HASH SHA256=42f6e95cad6ec532fd372391373363b62a14af6d771056dbfc86160e6dfff7aa
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(nlohmann_json)

# Monocypher 4.0.3, pinned to the immutable upstream commit. The optional
# Ed25519 source is compiled explicitly; the library has no CMake project.
FetchContent_Declare(
    monocypher
    GIT_REPOSITORY https://github.com/LoupVaillant/monocypher.git
    GIT_TAG ab2b16dd619ad5f6979a4fbe69cfa324a6fcc35f
    GIT_SHALLOW FALSE
)
FetchContent_GetProperties(monocypher)
if(NOT monocypher_POPULATED)
    FetchContent_Populate(monocypher)
endif()

add_library(VeyraMonocypher STATIC
    "${monocypher_SOURCE_DIR}/src/monocypher.c"
    "${monocypher_SOURCE_DIR}/src/optional/monocypher-ed25519.c"
)
target_include_directories(VeyraMonocypher PUBLIC
    "${monocypher_SOURCE_DIR}/src"
    "${monocypher_SOURCE_DIR}/src/optional"
)
set_target_properties(VeyraMonocypher PROPERTIES C_STANDARD 99 C_STANDARD_REQUIRED ON)

add_library(VeyraUpdaterCore STATIC
    src/manifest_verifier.cpp
    src/manifest_verifier.h
)
target_include_directories(VeyraUpdaterCore PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(VeyraUpdaterCore
    PUBLIC nlohmann_json::nlohmann_json
    PRIVATE VeyraMonocypher bcrypt
)
target_compile_features(VeyraUpdaterCore PUBLIC cxx_std_20)

add_executable(VeyraUpdater src/main.cpp)
target_link_libraries(VeyraUpdater PRIVATE VeyraUpdaterCore)
target_compile_features(VeyraUpdater PRIVATE cxx_std_20)

function(veyra_updater_warnings target_name)
    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

veyra_updater_warnings(VeyraUpdaterCore)
veyra_updater_warnings(VeyraUpdater)

if(BUILD_TESTING)
    add_executable(VeyraUpdaterTests tests/manifest_verifier_tests.cpp)
    target_link_libraries(VeyraUpdaterTests PRIVATE VeyraUpdaterCore VeyraMonocypher)
    target_compile_features(VeyraUpdaterTests PRIVATE cxx_std_20)
    veyra_updater_warnings(VeyraUpdaterTests)
    add_test(NAME VeyraUpdaterSecurityTests COMMAND VeyraUpdaterTests)
endif()

install(TARGETS VeyraUpdater RUNTIME DESTINATION .)
