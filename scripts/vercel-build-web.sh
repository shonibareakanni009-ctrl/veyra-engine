#!/usr/bin/env bash
#
# Veyra Engine — Vercel browser build
#
# Installs a pinned Emscripten toolchain, configures the existing CMake
# prototype, and compiles the HTML, JavaScript, and WebAssembly deliverables.
# This reuses the same C++ source and pinned SDL3 dependency as GitHub Actions.

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

echo "Veyra browser build completed. Static files are in $OUTPUT_ROOT"
