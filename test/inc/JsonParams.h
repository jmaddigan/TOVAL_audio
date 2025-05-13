#ifndef JSON_PARAMS_H
#define JSON_PARAMS_H

#include <nlohmann/json.hpp>
#include "TOVALaudio.h"
#include "TOVAL_Effect.h"
#include <map>
#include <vector>

// Forward declare the effect class and necessary enums
class TOVAL_Effect;
namespace Modules {
    enum ModuleID {
        GLOBAL = 0,
        HEADROOM,
        // Add other modules here
    };
    // Enum for Param IDs within the HEADROOM module
    namespace HeadroomParams {
        enum HeadroomParamID {
            ENABLE = 0,
            GAIN,
            // Add more ParamIDs for Headroom module
        };
    }

    namespace GlobalParams {
        enum GlobalParamID {
            ENABLE = 0,
            // Add more ParamIDs for Headroom module
        };
    }
}

// Define the mappings as external to be used across files
extern std::map<std::string, uint16_t> moduleNameToID;
extern std::map<std::string, uint16_t> paramNameToID;

// Function to load and set configuration from JSON
TOVAL_ERROR load_and_set_json_params(const nlohmann::json& jsonObj, TOVAL_Effect& effect);

#endif // JSON_PARAMS_H
