#ifndef TOVAL_EFFECT_P_H
#define TOVAL_EFFECT_P_H

#include "Headroom.h"
#include "TOVAL_Effect.h"  // Include the public header
#include "TOVALaudio.h"


// Define the struct that holds the private implementation
struct TOVAL_Effect::Impl {
    // Private member variables
    Headroom headroom;

    // Define Variables inside Impl
    struct Variables {
        uint32_t global_enable;
        uint32_t repeat_counter;
    } variables;  // Declare a member instance of Variables

    struct Config {
        float sample_rate;
        uint16_t In_num_channels;
        uint16_t Out_num_channels;

        // more to be added
    } config;   // potentially make public

    // Private methods
    TOVAL_ERROR TOVAL_Effect_do_set(uint32_t moduleID, uint16_t paramID, uint16_t data_length, void* data);
    TOVAL_ERROR TOVAL_Effect_do_get(uint32_t moduleID, uint16_t paramID, uint16_t data_length, void* data);

    void update_channel_config();

    TOVAL_ERROR global_set(uint16_t paramID, uint16_t data_length, void* data);
    
/*
    enum Modules {
        GLOBAL = 0,
        MODULE_FIRST = 1,
        HEADROOM = MODULE_FIRST,
        MODULE_COUNT    // always keep this last
    };

    enum GlobalParams {
        ENABLE
    };
*/
    int get_num_channels_for_module(TOVAL_Module module) {
    switch (module) {
        case HEADROOM: return headroom.num_channels;
        default:       return 0;
        }
    }
};

#endif // TOVAL_EFFECT_P_H
