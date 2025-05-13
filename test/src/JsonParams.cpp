#include "JsonParams.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

// Define the mappings
std::map<std::string, uint16_t> moduleNameToID = {
    {"HEADROOM", Modules::HEADROOM},
    {"GLOBAL", Modules::GLOBAL}
};

std::map<std::string, uint16_t> paramNameToID = {
    {"GAIN", Modules::HeadroomParams::GAIN},
    {"ENABLE", Modules::HeadroomParams::ENABLE},
    {"GLOBAL_ENABLE_FLAG", Modules::GlobalParams::ENABLE}
};  // work out how to split this into for each module

// Utility function to append primitive types into a byte array
template <typename T>
void append_to_bytes(std::vector<uint8_t>& buffer, T value) {
    uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
    buffer.insert(buffer.end(), ptr, ptr + sizeof(T));
}

// Converts a JSON object (with primitive values) into a byte array
std::vector<uint8_t> serialize_json_to_bytes(const nlohmann::json& obj) {
    std::vector<uint8_t> bytes;

    for (auto& [key, val] : obj.items()) {
        if (val.is_number_integer()) {
            int32_t i = val.get<int32_t>();
            append_to_bytes(bytes, i);
        } else if (val.is_number_unsigned()) {
            uint32_t u = val.get<uint32_t>();
            append_to_bytes(bytes, u);
        } else if (val.is_number_float()) {
            float f = val.get<float>();
            append_to_bytes(bytes, f);
        } else {
            std::cerr << "Unsupported JSON value type in param: " << key << std::endl;
        }
    }

    return bytes;
}

// Loads JSON and applies parameters via TOVAL_Effect::set
TOVAL_ERROR load_and_set_json_params(const nlohmann::json& jsonObj, TOVAL_Effect& effect) {
    TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;
    uint32_t count = 0;

    // Extract and print test case name
    std::string testName = jsonObj.value("test_case", "Unnamed Test");
    std::cout << "Initiating test case: " << testName << std::endl;

    for (auto& [moduleName, params] : jsonObj.items()) {
        if (moduleName == "test_case") {
            continue; // Skip test case metadata
        }

        // Find the module ID based on the module name
        auto moduleIt = moduleNameToID.find(moduleName);
        if (moduleIt == moduleNameToID.end()) {
            std::cerr << "Unknown module: " << moduleName << std::endl;
            continue;
        }

        uint16_t moduleID = moduleIt->second;
        std::cerr << "ModuleID: " << moduleID << std::endl;

        // Process each parameter in the module
        for (auto& [paramName, paramData] : params.items()) {
            // Find the parameter ID based on the parameter name
            auto paramIt = paramNameToID.find(paramName);
            if (paramIt == paramNameToID.end()) {
                std::cerr << "Unknown param: " << paramName << std::endl;
                continue;
            }

            uint16_t paramID = paramIt->second;

            // Convert paramData to raw byte buffer
            std::vector<uint8_t> rawData;

            // If the paramData is an object, serialize it into a byte array
            if (paramData.is_object()) {
                rawData = serialize_json_to_bytes(paramData);
            } else {
                // Handle known param types explicitly for scalar values
                if (paramName == "GLOBAL_ENABLE_FLAG") {
                    uint32_t uval = paramData.get<uint32_t>();
                    append_to_bytes(rawData, uval);
                } else if (paramName == "GAIN") {
                    float fval = paramData.get<float>();
                    append_to_bytes(rawData, fval);
                } else if (paramData.is_number_integer()) {
                    append_to_bytes(rawData, static_cast<int32_t>(paramData));
                } else if (paramData.is_number_unsigned()) {
                    append_to_bytes(rawData, static_cast<uint32_t>(paramData));
                } else if (paramData.is_number_float()) {
                    append_to_bytes(rawData, static_cast<float>(paramData));
                } else {
                    std::cerr << "Unsupported top-level param type: " << paramName << std::endl;
                    continue;
                }
            }

            // Create a pointer to the byte array data
            void* dataPtr = rawData.data();
            uint16_t dataLength = static_cast<uint16_t>(rawData.size());

            // Print the variable being set
            std::cout << "Setting [" << moduleName << "] -> [" << paramName << "] = ";
            if (paramData.is_number()) {
                std::cout << paramData;
            } else if (paramData.is_object()) {
                std::cout << paramData.dump();  // print object as string
            } else {
                std::cout << "(unknown format)";
            }
            std::cout << std::endl;

            // Call the public set function for the effect
            ret = effect.TOVAL_Effect_set(moduleID, paramID, dataLength, dataPtr);
            if (ret == TOVAL_ERROR::NO_ERROR) {
                ++count;
            } else {
                std::cerr << "Set failed for " << paramName << " (code: " << static_cast<int>(ret) << ")" << std::endl;
            }
        }
    }

    // If no parameters were applied, print a message
    if (count == 0) {
        std::cout << "No parameters applied for test case: " << testName << std::endl;
    }

    return ret;
}
