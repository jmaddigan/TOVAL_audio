# toolchain.cmake

# Set the CMake minimum required version
cmake_minimum_required(VERSION 3.10)

# Define the toolchain name (optional)
set(TOOLCHAIN_NAME "Default_Toolchain")

# Use the default compiler if no specific compiler is set
if(NOT DEFINED CMAKE_C_COMPILER)
    # Check for GCC first
    find_program(CMAKE_C_COMPILER gcc)
    # If GCC is not found, check for Clang
    if(NOT CMAKE_C_COMPILER)
        find_program(CMAKE_C_COMPILER clang)
    endif()
endif()

if(NOT DEFINED CMAKE_CXX_COMPILER)
    # Check for G++ first
    find_program(CMAKE_CXX_COMPILER g++)
    # If G++ is not found, check for Clang++
    if(NOT CMAKE_CXX_COMPILER)
        find_program(CMAKE_CXX_COMPILER clang++)
    endif()
endif()

# Optional: Set additional flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Optionally set the output directories for libraries and binaries
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Print information about the selected compiler
message(STATUS "Using C compiler: ${CMAKE_C_COMPILER}")
message(STATUS "Using C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "Toolchain name: ${TOOLCHAIN_NAME}")
