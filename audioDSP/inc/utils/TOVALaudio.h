#ifndef TOVALAUDIO_H
#define TOVALAUDIO_H

//Use ifdef to stop definition of enum twice if called mulitiple times

#include <cstdint>

#define TOVAL_VERSION_MAJOR 1
#define TOVAL_VERSION_MINOR 0
#define TOVAL_VERSION_PATCH 0

//#define NUM_CHANNELS 2
#define SAMPLE_RATE 48000
#define BIQUAD_NUM_COEFFS 5

enum class TOVAL_ERROR : std::uint32_t{
    NO_ERROR = 0,
    SIZE_ERROR,
    CONFIG_ERROR,
    PARAMID_ERROR,
    PARAMETER_ERROR,
    NULL_POINTER_ERROR,
    INPUT_WAV_ERROR,
    OUTPUT_WAV_ERROR,
    // Make full list of error codes
};

/*
enum class ModuleChannels {
    GAIN = 2, // e.g., 2 channels
    SOFTCLIP = 1, // e.g., 1 channel
    EQ = 2, // e.g., 2 channels
    ModuleD = 4, // e.g., 4 channels
    ModuleE = 1  // e.g., 1 channel
};
*/


// If need to add more global variables, make enum public part of bigger class

#endif