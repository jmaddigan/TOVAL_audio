#include <iostream>
#include "Headroom.h"
using namespace std;

TOVAL_ERROR Headroom::headroom_init()
{
    TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;
    cout<<"Headroom Init print"<<endl;

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
    cout << "Headroom Set print" << endl;

    ret = headroom_do_set(ParamID, data_length, data);

    if (ret != TOVAL_ERROR::NO_ERROR)
    {
        cout << "Error occurred in headroom_set: " << static_cast<int>(ret) << endl;
    }
    return ret;
}

TOVAL_ERROR Headroom::headroom_do_set(uint16_t ParamID, size_t data_length, void* data)
{
    TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;

    switch (ParamID)
    {
        case ENABLE:
            cout << "set Enable case called" << endl;
            ret = set_enable(data_length, data);
            break;

        case GAIN:
            cout << "set Gain case called" << endl;
            ret = set_gain(data_length, data);
            break;

        // Add other cases for other parameters...
        
        default:
            cout << "Error: No Parameter recognized for ParamID " << ParamID << endl;
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
        cout << "enable size error" << endl;
    }
    else if (data == nullptr)
    {
        ret = TOVAL_ERROR::NULL_POINTER_ERROR;
        cout << "nullptr error in enable" << endl;
    }
    else
    {
        bool value = *static_cast<const bool*>(data);
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
        std::cout << "size Error" << std::endl;
    }
    if (data == nullptr)
    {
        ret = TOVAL_ERROR::NULL_POINTER_ERROR;
        std::cout << "nullptr Error" << std::endl;
    }
    else
    {
        float values = *static_cast<const float*>(data);
        float gain = values;
        for(int ch=0; ch < HeadroomChannels::NUM_CHANNELS; ch++)
        {
            headroom_features[ch].gain = std::pow(10.0f, (gain / 20.0f));      // dB gain passed, Linear gain stored
            std::cout << "Gain for channel " << ch << " set to " << gain << std::endl;
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
    cout << "Headroom Get print" << endl;

    ret = headroom_do_get(ParamID, data_length, data);

    if (ret != TOVAL_ERROR::NO_ERROR)
    {
        cout << "Error occurred in headroom_get: " << static_cast<int>(ret) << endl;
    }
    return ret;
}

TOVAL_ERROR Headroom::headroom_do_get(uint16_t ParamID, size_t data_length, void* data)
{
    TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;

    switch (ParamID)
    {
        case ENABLE:
            ret = get_enable(data_length, data);
            break;

        // Add other cases for other parameters...

        default:
            cout << "Error: No Parameter recognized for ParamID " << ParamID << endl;
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
        *static_cast<bool*>(data) = enable;
    }

    return ret;
}



TOVAL_ERROR Headroom::headroom_process(float **ppIn, float **ppOut, size_t nspc) {
    TOVAL_ERROR error = TOVAL_ERROR::NO_ERROR;
    if (ppIn == nullptr || ppOut == nullptr)
    {
        std::cout << "Error: NULL pointer passed to headroom_process" << std::endl;
        return TOVAL_ERROR::NULL_POINTER_ERROR;
    }

    for (int ch = 0; ch < HeadroomChannels::NUM_CHANNELS; ++ch)
    {
        float* pIn = ppIn[ch];
        float* pOut = ppOut[ch];

        if (pIn == nullptr || pOut == nullptr)
        {
            std::cout << "Error: Channel pointer incorrect. Check channel mapping" << std::endl;
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

