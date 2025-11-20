#!/bin/bash
set -e

PROJECT_ROOT="../"
SERVER_SOURCE_DIR="${PROJECT_ROOT}backend"

# --- 2. Platform Detection and Variable Setup ---
OS_NAME=$(uname)
BUILD_SUBDIR=""
CMAKE_PRESET=""
JOB_COUNT=1

if [[ "$OS_NAME" == "Darwin" ]]; then
    # macOS (Homebrew setup)
    BUILD_SUBDIR="macos"
    CMAKE_PRESET="macos-homebrew"
    # Get physical CPU cores on macOS
    JOB_COUNT=$(sysctl -n hw.ncpu)
    echo "Detected macOS. Using ${JOB_COUNT} cores."
elif [[ "$OS_NAME" == "Linux" ]]; then
    # Linux (System setup)
    BUILD_SUBDIR="linux"
    CMAKE_PRESET="linux-system"
    # Get processor count on Linux
    JOB_COUNT=$(nproc)
    echo "Detected Linux. Using ${JOB_COUNT} cores."
else
    echo "Unsupported OS: ${OS_NAME}. Defaulting to 1 job."
fi

BUILD_DIR="${SERVER_SOURCE_DIR}/build/${BUILD_SUBDIR}"
EXECUTABLE_PATH="${BUILD_DIR}/fleet_server"

# --- 3. Configuration and Build ---

# Create build directory if it doesn't exist
echo "Creating build directory: ${BUILD_DIR}"
mkdir -p "$BUILD_DIR"

# Configure CMake using the appropriate preset
echo "Configuring C++ server using preset: ${CMAKE_PRESET}..."
cmake -S "${SERVER_SOURCE_DIR}" --preset "${CMAKE_PRESET}"

# Build server using CMake
echo "Building C++ server..."
# Use the native build tool (make) via the CMake wrapper
cmake --build "${BUILD_DIR}" -- -j"${JOB_COUNT}"

# --- 4. Run the server ---
if [ -f "$EXECUTABLE_PATH" ]; then
    echo "Starting server from: ${EXECUTABLE_PATH}"
    cd "$BUILD_DIR"
    "./fleet_server"
else
    echo "Error: Executable not found at ${EXECUTABLE_PATH}"
    exit 1
fi