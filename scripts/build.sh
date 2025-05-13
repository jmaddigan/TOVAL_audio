#!/bin/bash

# Path to the utils.h file where the version macros are defined
UTILS_H="../audioDSP/inc/utils/TOVALaudio.h"

# Extract the version numbers from the utils.h file
MAJOR_VERSION=$(grep -E '#define TOVAL_VERSION_MAJOR' "$UTILS_H" | awk '{print $3}')
MINOR_VERSION=$(grep -E '#define TOVAL_VERSION_MINOR' "$UTILS_H" | awk '{print $3}')
PATCH_VERSION=$(grep -E '#define TOVAL_VERSION_PATCH' "$UTILS_H" | awk '{print $3}')

# Check if version extraction succeeded
if [ -z "$MAJOR_VERSION" ] || [ -z "$MINOR_VERSION" ] || [ -z "$PATCH_VERSION" ]; then
    echo "Failed to extract version numbers from $UTILS_H"
    exit 1
fi

# Print the extracted version
echo "Building Tonal Valley Audio software version $MAJOR_VERSION.$MINOR_VERSION.$PATCH_VERSION"

# Parse command line options
DELIVERY_FLAG="OFF"
TOOLCHAIN_FILE=""
TOOLCHAIN_NAME="default"  # Default name for the toolchain

while getopts "dt:" opt; do
  case ${opt} in
    d )
      DELIVERY_FLAG="ON"
      ;;
    t )
      TOOLCHAIN_NAME=$OPTARG
      ;;
    \? )
      echo "Invalid option: $OPTARG" 1>&2
      exit 1
      ;;
  esac
done

# Determine the toolchain file based on the provided name
if [ -z "$TOOLCHAIN_NAME" ] || [ "$TOOLCHAIN_NAME" == "default" ]; then
    TOOLCHAIN_FILE="CMake/default_toolchain.cmake"  # Use default toolchain file
else
    TOOLCHAIN_FILE="CMake/${TOOLCHAIN_NAME}.cmake"  # Directly use the provided toolchain name
    echo "Looking for toolchain file: $TOOLCHAIN_FILE"  # This directs to other specified toolchain file. This bit needs debugging
    
    # Check if the specified toolchain file exists
    if [ ! -f "$TOOLCHAIN_FILE" ]; then
        echo "ERROR: Toolchain file not found: $TOOLCHAIN_FILE"
        exit 1
    fi
fi



# Navigate to build directory
BUILD_DIR="../build"

# Check if build directory exists
if [ ! -d "$BUILD_DIR" ]; then
    echo "Build directory does not exist. Running CMake configuration..."
    cmake -S .. -B "$BUILD_DIR" -DDELIVERY=$DELIVERY_FLAG -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE"
else
    echo "Build directory already exists. Running CMake with the specified delivery option."
    cmake -S .. -B "$BUILD_DIR" -DDELIVERY=$DELIVERY_FLAG -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE"
fi

# Always run the build command
cmake --build "$BUILD_DIR"

# Install the libraries to the delivery folder if the DELIVERY option is ON
if [ "$DELIVERY_FLAG" == "ON" ]; then
    echo "Installing libraries to the delivery folder..."
    cmake --install "$BUILD_DIR"
else
    echo "No installation performed as DELIVERY is OFF."
fi
