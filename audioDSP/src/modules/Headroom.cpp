#include <iostream>
#include "Headroom.h"
using namespace std;

TOVAL_ERROR Headroom::headroom_init()
{
    TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;

    headroom_features.resize(NUM_CHANNELS);       // In future this will not be macro, but a variable in main effect, defined in main effect Init before this
    y_1.resize(NUM_CHANNELS);                     // Using vector allows num channels to be more flexible at runtime.
    for(int ch=0; ch<headroom_features.size(); ch++)
    {
        headroom_features[ch].channel = ch;
        headroom_features[ch].gain = 1;
        y_1[ch] = 0;
    }
    enable = 0;
    alpha = 0.2f;
  return ret;
}

TOVAL_ERROR Headroom::headroom_set(uint16_t ParamID, size_t data_length, void* data)
{
    TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;

    ret = headroom_do_set(ParamID, data_length, data);

    return ret;
}

TOVAL_ERROR Headroom::headroom_do_set(uint16_t ParamID, size_t data_length, void* data)
{
    TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;

    switch (ParamID)
    {
        case TOVAL_HeadroomParam::HR_ENABLE:
            ret = set_enable(data_length, data);
            break;

        case TOVAL_HeadroomParam::HR_GAIN:
            ret = set_gain(data_length, data);
            break;

        default:
            ret = TOVAL_ERROR::PARAMID_ERROR;
            break;
    }
    return ret;
}

TOVAL_ERROR Headroom::set_enable(size_t data_length, void* data)
{
    TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;

    if (data_length != sizeof(enable))
    {
        ret = TOVAL_ERROR::SIZE_ERROR;
    }
    else if (data == nullptr)
    {
        ret = TOVAL_ERROR::NULL_POINTER_ERROR;
    }
    else
    {
        bool value = *static_cast<const uint32_t*>(data);
        enable = value;
    }
    return ret;
}

TOVAL_ERROR Headroom::set_gain(size_t data_length, void* data)
{
    TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;
    if (data_length != sizeof(float))
    {
        ret = TOVAL_ERROR::SIZE_ERROR;
    }
    if (data == nullptr)
    {
        ret = TOVAL_ERROR::NULL_POINTER_ERROR;
    }
    else
    {
        float values = *static_cast<const float*>(data);
        float gain = values;
        for(int ch=0; ch < HeadroomChannels::NUM_CHANNELS; ch++)
        {
            headroom_features[ch].gain = std::pow(10.0f, (gain / 20.0f));      // dB gain passed, Linear gain stored
        }
    }
    return ret;
}

TOVAL_ERROR Headroom::set_alpha(size_t data_length, void* data)
{
    TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;
    if (data_length != sizeof(alpha))
    {
        ret = TOVAL_ERROR::SIZE_ERROR;
    }

    else if (data == nullptr)
    {
        ret = TOVAL_ERROR::NULL_POINTER_ERROR;
    }
    else
    {
        bool value = *static_cast<const float*>(data);
        alpha = value;
    }
    return ret;
}
TOVAL_ERROR Headroom::headroom_get(uint16_t ParamID, size_t data_length, void* data)
{
    TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;

    ret = headroom_do_get(ParamID, data_length, data);

    return ret;
}

TOVAL_ERROR Headroom::headroom_do_get(uint16_t ParamID, size_t data_length, void* data)
{
    TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;

    switch (ParamID)
    {
        case TOVAL_HeadroomParam::HR_ENABLE:
            ret = get_enable(data_length, data);
            break;

        // Add other cases for other parameters...

        default:
            ret = TOVAL_ERROR::PARAMID_ERROR;
            break;
    }

    return ret;
}

TOVAL_ERROR Headroom::get_enable(size_t data_length, void* data)
{
    TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;

    // Check if the provided data length matches the size of enable
    if (data_length != sizeof(enable))
    {
        ret = TOVAL_ERROR::SIZE_ERROR;
    }
    else if (data == nullptr)
    {
        ret = TOVAL_ERROR::NULL_POINTER_ERROR;
    }
    else
    {
        // Cast data to bool pointer and retrieve the enable value
        *static_cast<uint32_t*>(data) = enable;
    }

    return ret;
}

TOVAL_ERROR Headroom::get_gain(size_t data_length, void* data)
{
    TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;
    if (data_length != sizeof(float))
    {
        ret = TOVAL_ERROR::SIZE_ERROR;
    }
    if (data == nullptr)
    {
        ret = TOVAL_ERROR::NULL_POINTER_ERROR;
    }
    else
    {
        float value = 20.0f * std::log10(headroom_features[0].gain);
        *static_cast<float*>(data) = value;      // dB gain passed, Linear gain stored
    }

    return ret;
}



TOVAL_ERROR Headroom::headroom_process(float **ppIn, float **ppOut, size_t nspc) {
    TOVAL_ERROR error = TOVAL_ERROR::NO_ERROR;
    if (ppIn == nullptr || ppOut == nullptr)
    {
        return TOVAL_ERROR::NULL_POINTER_ERROR;
    }

    for (int ch = 0; ch < HeadroomChannels::NUM_CHANNELS; ++ch)
    {
        float* pIn = ppIn[ch];
        float* pOut = ppOut[ch];

        if (pIn == nullptr || pOut == nullptr)
        {
            return TOVAL_ERROR::NULL_POINTER_ERROR;
        }

        if (!enable)
        {
            std::memcpy(pOut, pIn, sizeof(float) * nspc);
            continue;  // Skip processing for this channel
        }

        float gain = headroom_features[ch].gain;

        for (int sample = 0; sample < nspc; ++sample)
        {
            float smoothed = (1 - alpha) * pIn[sample] + alpha * y_1[ch];
            y_1[ch] = smoothed;
            pOut[sample] = smoothed * gain;  // Apply gain after smoothing
        }
    }

    return error;
}

