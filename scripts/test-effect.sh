#!/bin/bash

# Define the test executable directory
source ../build/config.txt
TEST_EXE_DIR="../build/bin"
TEST_EXE="$TEST_EXE_DIR/$TOVAL_EXE"

# Define the test cases directory
TEST_CASES_DIR="../test/test_cases"
QA_DIR="../QA"

# Check if the test executable exists
if [ ! -f "$TEST_EXE" ]; then
    echo "ERROR: Test executable not found at $TEST_EXE"
    exit 1
fi

# Create QA output directory
mkdir -p "$QA_DIR"

echo "..Running TOVAL Audio Tests..."

# Iterate over test case directories
for TEST_CASE in "$TEST_CASES_DIR"/*; do
    if [ -d "$TEST_CASE" ]; then
        CASE_NAME=$(basename "$TEST_CASE")
        CASE_QA_DIR="$QA_DIR/$CASE_NAME"
        mkdir -p "$CASE_QA_DIR"

        echo "Running test for case: $CASE_NAME"

        # Redirect stdout to a log file and pass QA path as second arg
        "$TEST_EXE" "$TEST_CASE" "$CASE_QA_DIR" > "$CASE_QA_DIR/${CASE_NAME}.log" 2>&1
    fi
done

echo "All tests completed."
