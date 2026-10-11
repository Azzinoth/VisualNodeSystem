#!/usr/bin/env bash
# Builds and runs Visual Node System tests on macOS (Apple Silicon), then prints a summary.
# Usage: run-tests-macos.sh [Debug|Release]
# Exit codes: 0 - all tests passed, 1 - some tests failed, 2 - required tools are missing, 3 - build failed.
# Logs and Results.xml are written to build-tests/macOS.

case "$1" in
	Debug|debug) CONFIG="Debug" ;;
	""|Release|release) CONFIG="Release" ;;
	*) echo "Usage: run-tests-macos.sh [Debug|Release]"; exit 2 ;;
esac

REPO_DIR="$(cd "$(dirname "$0")" && pwd)"
OUTPUT_DIR="$REPO_DIR/build-tests/macOS"
BUILD_DIR="$OUTPUT_DIR/build-$CONFIG"
RUN_DIR="$OUTPUT_DIR/run-$CONFIG"

mkdir -p "$OUTPUT_DIR"

# CMake.app from cmake.org does not add cmake to PATH.
CMAKE="$(command -v cmake || echo /Applications/CMake.app/Contents/bin/cmake)"
MISSING=""
[ -x "$CMAKE" ] || MISSING="$MISSING cmake"
# /usr/bin/clang++ exists even without developer tools, xcrun fails when they are not installed.
xcrun --find clang++ > /dev/null 2>&1 || MISSING="$MISSING clang++"
if [ -n "$MISSING" ]; then
	echo "Missing:$MISSING"
	echo "Install CMake from https://cmake.org/download and the developer tools with: xcode-select --install"
	exit 2
fi

echo "Configuring and building $CONFIG..."
if ! { "$CMAKE" -S "$REPO_DIR" -B "$BUILD_DIR" -G "Unix Makefiles" -DCMAKE_BUILD_TYPE="$CONFIG" && "$CMAKE" --build "$BUILD_DIR" -j "$(sysctl -n hw.ncpu)"; } > "$OUTPUT_DIR/build.log" 2>&1; then
	echo "Build failed, see build-tests/macOS/build.log"
	exit 3
fi

echo "Running tests..."
rm -rf "$RUN_DIR" && mkdir -p "$RUN_DIR" && cp -R "$REPO_DIR/Resources" "$RUN_DIR/"
cd "$RUN_DIR" || exit 3
"$BUILD_DIR/Visual_Node_System_Tests" > "$OUTPUT_DIR/test.log" 2>&1
TEST_EXIT_CODE=$?
cp Results.xml "$OUTPUT_DIR/" 2> /dev/null

# googletest ends a run with a summary that lists the failed tests, it is missing if the run crashed.
SUMMARY="$(sed -n '/^\[==========\] .* ran\./,$p' "$OUTPUT_DIR/test.log")"
echo "${SUMMARY:-Tests did not finish (exit code $TEST_EXIT_CODE), see build-tests/macOS/test.log}"
echo "Logs and Results.xml: build-tests/macOS"

[ $TEST_EXIT_CODE -eq 0 ] && exit 0 || exit 1
