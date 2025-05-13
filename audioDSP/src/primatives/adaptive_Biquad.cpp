#include <iostream>
#include "adaptive_Biquad.h"
#include "conversionFN.h"
using namespace std;

TOVAL_ERROR AdaptiveBiquad::AdaptiveBiquad_init(uint16_t channelID)
{
    TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;
    this->channelID = channelID;

    cout<<"Adaptive Biquad Init print for channel " << this->channelID <<endl;

    xz1 = 0;
    xz2 = 0;
    yz1 = 0;
    yz2 = 0;
    return ret;
}

 TOVAL_ERROR AdaptiveBiquad::set_minBiquadCoeff(size_t data_length, void* data)
 {
    TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;
    if(data_length == sizeof(biquadcoeff))
    {
        minBiquadCoeffs = *reinterpret_cast<biquadcoeff*>(data);
    }
    else{
        cout << "Biquad size error" << endl;
        ret = TOVAL_ERROR::SIZE_ERROR;
    }
    return ret;
 }

  TOVAL_ERROR AdaptiveBiquad::set_maxBiquadCoeff(size_t data_length, void* data)
 {
    TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;
    if(data_length == sizeof(biquadcoeff))
    {
        maxBiquadCoeffs = *reinterpret_cast<biquadcoeff*>(data);
    }
    else{
        cout << "Biquad size error" << endl;
        ret = TOVAL_ERROR::SIZE_ERROR;
    }
    return ret;
 }

TOVAL_ERROR AdaptiveBiquad::set_minGain(size_t data_length, void* data)
{
    TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;

    // Max gain passed as linear value

    if(data_length == sizeof(maxGain))
    {
        minGain = *reinterpret_cast<float*>(data);
    }
    else
    {
        ret = TOVAL_ERROR::SIZE_ERROR;
    }
    return ret;
}

 TOVAL_ERROR AdaptiveBiquad::set_maxGain(size_t data_length, void* data)
{
    TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;

    // Max gain passed as linear value

    if(data_length == sizeof(maxGain))
    {
        maxGain = *reinterpret_cast<float*>(data);
    }
    else
    {
        ret = TOVAL_ERROR::SIZE_ERROR;
    }
    return ret;
}


TOVAL_ERROR AdaptiveBiquad::set_stepResponse(size_t data_length, void* data)
{
    TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;
    /*
        Currently, there is just a scale of 0.3 and 0.9
        But in future, I want to group the scale into 3 groups, you can select Slow, Medium or fast.
            Fast EQ changes (e.g., live instruments, quick dynamics)
            Moderate transitions (e.g., vocal processing, smooth filtering)
            Slow, gradual gain changes (e.g., auto-EQ, AGC, ambiance control)
        You can then make finer adjustments within these sub groups
    */
    if(data_length == sizeof(alpha))
    {
        alpha = *reinterpret_cast<float*>(data);
    }
    else
    {
        ret = TOVAL_ERROR::SIZE_ERROR;
    }
    if((alpha < alphaMin) || (alpha > alphaMax))
    {
        ret = TOVAL_ERROR::PARAMETER_ERROR;
        cout << "step Response Alpha value out of Range. Returning Parameter Error " << endl;
    }
    return ret;
}

TOVAL_ERROR AdaptiveBiquad::set_CurrentBiquadCoeff(biquadcoeff &currentBiquad, biquadcoeff minCoeffs, biquadcoeff maxCoeffs,  float scale)
{
    TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;

    currentBiquad.coeff[0] = (1 - scale) * minCoeffs.coeff[0] + scale * maxCoeffs.coeff[0];
    currentBiquad.coeff[1] = (1 - scale) * minCoeffs.coeff[1] + scale * maxCoeffs.coeff[1];
    currentBiquad.coeff[2] = (1 - scale) * minCoeffs.coeff[2] + scale * maxCoeffs.coeff[2];
    currentBiquad.coeff[3] = (1 - scale) * minCoeffs.coeff[3] + scale * maxCoeffs.coeff[3];
    currentBiquad.coeff[4] = (1 - scale) * minCoeffs.coeff[4] + scale * maxCoeffs.coeff[4];

    return ret;

}

TOVAL_ERROR AdaptiveBiquad::interpolateCurrentGain(size_t data_length, float *ppIn) 
{
    TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;
    float sumGainDB = 0.0f;

    // Convert linear input to dB and sum up
    for (size_t i = 0; i < data_length; i++) 
    {
        sumGainDB += linearToDB(ppIn[i]);  // Convert linear input to dB
    }

    // Compute the average gain in dB
    float avgGainDB = sumGainDB / static_cast<float>(data_length);

    // Ensure values are within bounds in the dB domain
    float rawScale;
    if (avgGainDB >= maxGain) {
        rawScale = 1.0f;
    } 
    else if (avgGainDB <= minGain) {
        rawScale = 0.0f;
    } 
    else {
        rawScale = (avgGainDB - minGain) / (maxGain - minGain);  // Linear scaling in dB
    }

    // Apply step response smoothing for gradual transitions
    this->currentScale = stepResponse(rawScale, this->currentScale, alpha);  

    return ret;
}

TOVAL_ERROR AdaptiveBiquad::AdaptiveBiquad_process(float *pIn, float *pOut, size_t nspc)
{
    TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;

    biquadcoeff currentCoeffs;
    
    ret = interpolateCurrentGain(nspc, pIn);     // current scale is set

    ret = set_CurrentBiquadCoeff(currentCoeffs, minBiquadCoeffs, maxBiquadCoeffs, currentScale);

    for(int i = 0; i < nspc; i++)
    {
        pOut[i] = (currentCoeffs.coeff[0] * pIn[i]) + (currentCoeffs.coeff[1] * xz1) + (currentCoeffs.coeff[2] * xz2) + (currentCoeffs.coeff[3] * yz1) + (currentCoeffs.coeff[4] * yz2);

        xz2 = xz1;
        xz1 = pIn[i];

        yz2 = yz1;
        yz1 = pOut[i];
    }
    
    return ret;
}

// Add potential error trips




// open below
/*
    The aim of this module is to calculate the input level in decibels, then calculate a ratio between an min and max value.
    This value is then use to calculate interpolated biquad coefficients between what the user sets for the max level EQ and the lowest level EQ

    If gain is above max, = max. If below min, = min (only for the coefficients, not the actual gain).
    
    After the set functions set coefficients

    The EQ module calls the get currentBiquad coefficient constenty every process call when in adaptive mode
  
    Internal Variables:
    - Converted dB level
    - Ratio
    - 

    Functions:
    - SetMaxGain (called by EQ, sets the max gain level, used for calculating biquad interpolated ratio internally)
    - SetMinGain (called by EQ, sets the min gain level, used for calculating biquad interpolated ratio internally)
    - SetStepResponse (called by EQ, sets the response alpha value, determining how fast responsive the EQ is to gain, need to work out a range in test)
    - SetMaxBiquadCoeff (called by EQ, stores max biquad coefficients internally)
    - SetMinBiquadCoeff (called by EQ, stores min biquad coefficients internally)
    - GetCurrentBiquad (called by EQ, returns the current interpolated coefficients based on input level
                        , need to pass input level as an argument, and a pointer to fill with the coefficients)  
*/