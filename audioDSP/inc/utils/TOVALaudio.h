#ifndef TOVALAUDIO_H
#define TOVALAUDIO_H

#include <cstdint>
#include <cstddef>

// Versioning
#define TOVAL_VERSION_MAJOR 1
#define TOVAL_VERSION_MINOR 0
#define TOVAL_VERSION_PATCH 0

enum class TOVAL_ERROR : std::uint32_t {
    NO_ERROR = 0,
    SIZE_ERROR,
    CONFIG_ERROR,
    MODULEID_ERROR,
    PARAMID_ERROR,
    PARAMETER_ERROR,
    NULL_POINTER_ERROR,
    INPUT_WAV_ERROR,
    OUTPUT_WAV_ERROR
};

// ----------- Modules ------------
enum TOVAL_Module : uint16_t {
    GLOBAL = 0,
    MODULE_FIRST = 1,
    HEADROOM = MODULE_FIRST,
    MODULE_COUNT  // always last
};

// ---------- Global Params ---------
enum TOVAL_GlobalParam : uint16_t {
    GLOBAL_ENABLE = 0
};

// ---------- Headroom Params -------
enum TOVAL_HeadroomParam : uint16_t {
    HR_ENABLE = 0,
    HR_GAIN
};

#endif // TOVALAUDIO_H
