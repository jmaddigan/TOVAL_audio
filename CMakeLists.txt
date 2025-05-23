cmake_minimum_required(VERSION 3.29)

project(TOVALAudioDEV VERSION 1.0.0 LANGUAGES CXX)     # can remove version, defined in TOVALaudio.h

set(CMAKE_CXX_STANDARD          20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS       OFF)

set(TOOLCHAIN_NAME "${TOOLCHAIN_NAME}")
message(STATUS "Using toolchain: ${TOOLCHAIN_NAME}")

set(TOVAL_LIB TOVAL_Effect)         # Set audio effect static lib name
set(TOVAL_EXE TOVAL_Effect_test)    # Set audio effect test executable name

file(WRITE "${CMAKE_BINARY_DIR}/config.txt" "TOVAL_EXE=${TOVAL_EXE}\n")

set(SOFTCLIP_LIB softclip_lib)    # Set audio module static lib name
set(MODULE_TESTS module_tests)

option(DELIVERY "option to add library to delivery folder" OFF)

add_subdirectory(audioDSP/src)
add_subdirectory(audioDSP/inc)
add_subdirectory(test/src)
add_subdirectory(test/inc)
#add_subdirectory(test/test_cases)

# Add include and library directories
include_directories(/usr/local/include)
link_directories(/usr/local/lib)

# Find and link libsndfile
find_path(SNDFILE_INCLUDE_DIR sndfile.h PATHS /usr/local/include)
find_library(SNDFILE_LIBRARY sndfile PATHS /usr/local/lib)

if (SNDFILE_INCLUDE_DIR AND SNDFILE_LIBRARY)
    message(STATUS "Found libsndfile: ${SNDFILE_LIBRARY}")
else()
    message(FATAL_ERROR "libsndfile not found! Check installation.")
endif()

# Link it to the test executable
target_include_directories(${TOVAL_EXE} PRIVATE ${SNDFILE_INCLUDE_DIR})
target_link_libraries(${TOVAL_EXE} ${SNDFILE_LIBRARY})