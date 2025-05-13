#!/bin/bash

# Define the test executable directory
TEST_EXE_DIR="../build/bin"
TEST_EXE="$TEST_EXE_DIR/module_tests"  # @TOVAL_EXE1@ will be replaced by CMake with the actual executable name

# create if condition that changes the path of TEST_EXE to each module executable, if that option is selected as sh variable

# Check if the test executable directory exists
if [ ! -d "$TEST_EXE_DIR" ]; then
    echo "ERROR: Test executable directory not found"
    exit 1  # Exit with error status
else
    echo "..Running TOVAL Audio Test..."

    # Check if the test executable exists
    if [ -f "$TEST_EXE" ]; then
        echo "Executing test: $TEST_EXE"
        "$TEST_EXE"  # Run the test executable
    else
        echo "ERROR: Test executable not found at $TEST_EXE"
        exit 1
    fi
fi
