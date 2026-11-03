#!/usr/bin/env bash
#
# Veyra Engine website build for Vercel.
#
# The public website is static HTML/CSS/JavaScript. Adventure and SFS are
# standalone browser games; neither is an export from the native engine yet.
# Keep the output directory stable because it matches the existing Vercel
# project's configured output path.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUTPUT="$ROOT/build/vercel-browser/bin"
SITE="$ROOT/website"

test -s "$SITE/index.html"
test -s "$SITE/styles.css"
test -s "$SITE/game/index.html"
test -s "$SITE/game/game.css"
test -s "$SITE/game/game.js"
test -s "$SITE/sfs/index.html"
test -s "$SITE/sfs/sfs.css"
test -s "$SITE/sfs/sfs.js"

rm -rf "$OUTPUT"
mkdir -p "$OUTPUT"
cp -R "$SITE/." "$OUTPUT/"
test -s "$OUTPUT/index.html"
test -s "$OUTPUT/game/index.html"
test -s "$OUTPUT/game/game.js"
test -s "$OUTPUT/sfs/index.html"
test -s "$OUTPUT/sfs/sfs.css"
test -s "$OUTPUT/sfs/sfs.js"

echo "Veyra website and browser game are ready in $OUTPUT"
