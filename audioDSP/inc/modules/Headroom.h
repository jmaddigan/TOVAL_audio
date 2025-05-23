#ifndef HEADROOM_H
#define HEADROOM_H

#include <cstdint>
#include <math.h>
#include <vector>

#include "TOVALaudio.h"
#include "conversionFN.h"


enum HeadroomChannels
    {
        LEFT,
        RIGHT,
        NUM_CHANNELS
    };

class Headroom {

    public:
 
    TOVAL_ERROR headroom_init();
    TOVAL_ERROR headroom_set(uint16_t ParamID, size_t data_length, void* data);
    TOVAL_ERROR headroom_get(uint16_t ParamID, size_t data_length, void* data);
    TOVAL_ERROR headroom_process(float **ppIn, float **ppOut, size_t nspc);

    uint16_t num_channels = HeadroomChannels::NUM_CHANNELS;
    /*
        Don't need the module ID for inside here. For loop itterating through each module ID is done in Delay effect do_set,
        therefor if module ID is soft clip, EQ_do_set just needs to itterate through param ID's

        Look into whether I need to include set to do_set function call.
    */


    private:

    TOVAL_ERROR headroom_do_set(uint16_t ParamID, size_t data_length, void* data);

    TOVAL_ERROR set_enable(size_t data_length, void* data); 
    TOVAL_ERROR set_gain(size_t data_length, void* data);
    TOVAL_ERROR set_alpha(size_t data_length, void* data);
    

    TOVAL_ERROR headroom_do_get(uint16_t ParamID, size_t data_length, void* data);
    // Gets need adapting
    TOVAL_ERROR get_enable(size_t data_length, void* data);
    TOVAL_ERROR get_gain(size_t data_length, void* data);
    TOVAL_ERROR get_stepResponse(size_t data_length, void* data);


    uint32_t enable;       // Check where ref code stores enable variable, and how it passes data

    //std::array<float, HeadroomChannels::NUM_CHANNELS> y_1 = {};
    std::vector<float> y_1;
    float gain;
    float alpha;

    struct Headroom_gain
    {
        uint32_t channel;
        float gain;
    };  // structure must be same order as elements passed in Json file for testing purposes

    std::vector<Headroom_gain> headroom_features;


/*
    enum Params
    {
        ENABLE,
        GAIN,
        STEP_RESPONSE
    };
*/
};
// Look into using 16 bit integers for arguments. also using template instead of void* for data


#endif //MODULE1_H
