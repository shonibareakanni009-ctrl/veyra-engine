#!/usr/bin/env bash
#
# Veyra Engine — Vercel browser build
#
# Installs a pinned Emscripten toolchain, configures the existing CMake
# prototype, and compiles the HTML, JavaScript, and WebAssembly deliverables.
# This reuses the same C++ source and pinned SDL3 dependency as GitHub Actions.
#
# IMPORTANT: Vercel serves the root URL ("/") from a file named index.html.
# Emscripten names its generated page after the CMake executable target, so
# this script also publishes a copy as index.html. The generated page keeps
# its original JavaScript/WASM filenames; those files remain beside it.

set -euo pipefail

REPOSITORY_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_ROOT="$REPOSITORY_ROOT/build/vercel-browser"
OUTPUT_ROOT="$BUILD_ROOT/bin"
TOOL_ROOT="$REPOSITORY_ROOT/.vercel-tools"
EMSDK_ROOT="$TOOL_ROOT/emsdk"

# Install CMake and Ninja into an isolated directory instead of depending on
# undocumented versions that might happen to exist in Vercel's build image.
python3 -m pip install --disable-pip-version-check --no-warn-script-location \
    --target "$TOOL_ROOT/python" \
    "cmake>=3.28,<4" \
    "ninja>=1.11,<2"

export PYTHONPATH="$TOOL_ROOT/python:"
export PATH="$TOOL_ROOT/python/bin:$TOOL_ROOT/python/cmake/data/bin:$PATH"

# Vercel may restore a build cache containing a partially-created emsdk
# directory from an interrupted download. A directory without .git is not a
# usable SDK checkout, so remove only that incomplete checkout before cloning.
if [[ -d "$EMSDK_ROOT" && ! -d "$EMSDK_ROOT/.git" ]]; then
    echo "Removing incomplete cached Emscripten SDK checkout."
    rm -rf "$EMSDK_ROOT"
fi

# Install the same Emscripten SDK version used by the GitHub Actions build.
if [[ ! -d "$EMSDK_ROOT/.git" ]]; then
    mkdir -p "$TOOL_ROOT"
    git clone --depth 1 --branch 6.0.10 \
        https://github.com/emscripten-core/emsdk.git \
        "$EMSDK_ROOT"
fi

cd "$EMSDK_ROOT"
./emsdk install 6.0.10
./emsdk activate 6.0.10
# shellcheck disable=SC1091
source ./emsdk_env.sh

# Build from the repository root; Vercel publishes only the bin/ output.
cd "$REPOSITORY_ROOT"
emcmake cmake \
    -S prototypes/browser \
    -B "$BUILD_ROOT" \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY="$OUTPUT_ROOT"

cmake --build "$BUILD_ROOT" --parallel 2

# Refuse to publish if any of the three required browser files is missing.
test -s "$OUTPUT_ROOT/veyra_browser_prototype.html"
test -s "$OUTPUT_ROOT/veyra_browser_prototype.js"
test -s "$OUTPUT_ROOT/veyra_browser_prototype.wasm"

# Emscripten's HTML filename follows the executable target name. Vercel does
# not automatically discover that custom filename for the site's root route.
# Copy (do not rename) the page to index.html so "/" has a default document.
# The page still loads the target-named .js and .wasm files from this directory.
cp "$OUTPUT_ROOT/veyra_browser_prototype.html" "$OUTPUT_ROOT/index.html"
test -s "$OUTPUT_ROOT/index.html"

echo "Veyra browser build completed. Static files are in $OUTPUT_ROOT"
echo "Vercel root document: $OUTPUT_ROOT/index.html"
