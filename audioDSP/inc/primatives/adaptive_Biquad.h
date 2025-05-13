#ifndef ADAPTIVEBIQUAD_H
#define ADAPTIVEBIQUAD_H

#include "TOVALaudio.h"
#include <cstddef>
#include <cstdint>
#include <math.h>


class AdaptiveBiquad {

    public:     // Do I need private functions?
    
    TOVAL_ERROR AdaptiveBiquad_init(uint16_t channelID);
    TOVAL_ERROR AdaptiveBiquad_process(float *pIn, float *pOut, size_t nspc);


    TOVAL_ERROR set_minBiquadCoeff(size_t data_length, void* data);
    TOVAL_ERROR set_maxBiquadCoeff(size_t data_length, void* data);

    TOVAL_ERROR set_minGain(size_t data_length, void* data);                   // Done
    TOVAL_ERROR set_maxGain(size_t data_length, void* data);                   // Done
    TOVAL_ERROR set_stepResponse(size_t data_length, void* data);              // Done
    TOVAL_ERROR interpolateCurrentGain(size_t data_length, float *ppIn);       // Done
    


    TOVAL_ERROR set_MinMaxBiquadCoeff(size_t data_length, void* data);               // Done
 
    TOVAL_ERROR get_minGain(size_t data_length, void* data);
    TOVAL_ERROR get_maxGain(size_t data_length, void* data);
    TOVAL_ERROR get_minBiquadCoeff(size_t data_length, void* data);
    TOVAL_ERROR get_maxBiquadCoeff(size_t data_length, void* data);
    TOVAL_ERROR get_stepResponse(size_t data_length, void* data);

    TOVAL_ERROR interpolateCoefficients(size_t data_length, void* data);     // called by public get function. fills data with current calculated coefficients
                                                                          // declares local biquadcoeff struct object, does interpolation calculation, 
                                                                          // then fills data pointer with local biquad coeff values (interpolated from min and max variables)

    private:

    uint16_t channelID;

    float minGain;
    float maxGain;
    float currentScale;
    float alpha;
    float alphaMin = 0.3;
    float alphaMax = 0.9;
    
    enum Filter
    {
        MIN_FILTER,
        MAX_FILTER
    };

    struct biquadcoeff
    {
        float coeff[BIQUAD_NUM_COEFFS];
    };

    float xz1;   // initialise to 0 in init function
    float xz2;
    float yz1;
    float yz2;

    biquadcoeff minBiquadCoeffs;
    biquadcoeff maxBiquadCoeffs;

    TOVAL_ERROR set_CurrentBiquadCoeff(biquadcoeff &currentBiquad, biquadcoeff minCoeffs, biquadcoeff maxCoeffs,  float scale);

};

// Look into using 16 bit integers for arguments. also using template instead of void* for data

#endif //ADAPTIVEBIQUAD_H
