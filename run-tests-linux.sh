#!/usr/bin/env bash
# Builds and runs Visual Node System tests on Linux, run-tests.ps1 starts it through WSL.
# Usage: run-tests-linux.sh <Debug|Release>
# Exit codes: 0 - all tests passed, 1 - some tests failed, 2 - required tools are missing, 3 - build failed.

CONFIG="${1:-Release}"
REPO_DIR="$(cd "$(dirname "$0")" && pwd)"
OUTPUT_DIR="$REPO_DIR/build-tests/Linux"
# Building and running inside the Linux file system is much faster than on a Windows drive.
WORK_DIR="$HOME/.cache/VisualNodeSystemTests/$(printf '%s' "$REPO_DIR" | md5sum | cut -c1-8)"
BUILD_DIR="$WORK_DIR/build-$CONFIG"
RUN_DIR="$WORK_DIR/run-$CONFIG"

mkdir -p "$OUTPUT_DIR"

MISSING=""
for Tool in cmake ninja g++; do
	command -v "$Tool" > /dev/null || MISSING="$MISSING $Tool"
done
for Header in GL/gl.h X11/Xlib.h X11/extensions/Xrandr.h; do
	[ -f "/usr/include/$Header" ] || MISSING="$MISSING $Header"
done
if [ -n "$MISSING" ]; then
	echo "Missing:$MISSING"
	echo "Install with: sudo apt install cmake ninja-build g++ libx11-dev libxrandr-dev libgl-dev"
	exit 2
fi

echo "Configuring and building $CONFIG..."
if ! { cmake -S "$REPO_DIR" -B "$BUILD_DIR" -G Ninja -DCMAKE_BUILD_TYPE="$CONFIG" && cmake --build "$BUILD_DIR"; } > "$OUTPUT_DIR/build.log" 2>&1; then
	echo "Build failed, see build-tests/Linux/build.log"
	exit 3
fi

echo "Running tests..."
rm -rf "$RUN_DIR" && mkdir -p "$RUN_DIR" && cp -r "$REPO_DIR/Resources" "$RUN_DIR/"
cd "$RUN_DIR" || exit 3
"$BUILD_DIR/Visual_Node_System_Tests" > "$OUTPUT_DIR/test.log" 2>&1
TEST_EXIT_CODE=$?
cp Results.xml "$OUTPUT_DIR/" 2> /dev/null

[ $TEST_EXIT_CODE -eq 0 ] && exit 0 || exit 1
