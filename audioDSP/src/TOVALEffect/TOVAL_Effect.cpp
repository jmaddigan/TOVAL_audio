#include "TOVAL_Effect_p.h"
#include <iostream>

using namespace std;

TOVAL_Effect::TOVAL_Effect() : pImpl(new Impl) {}   // constructor is called when new object of class TOVAL_delay is called. This then initialises a new object of the internal struct 'Impl' named pImpl

TOVAL_Effect::~TOVAL_Effect() {
    delete pImpl;
}

TOVAL_ERROR TOVAL_Effect::TOVAL_Effect_init()
{
  TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;
  cout<<"Calling first module in TOVAL init"<<endl;

  // Config
  //pImpl->config.num_channels = NUM_CHANNELS;
  
  // call a set function that sets number samples per channel (chunk size). Can pass buffer.getNumSamples from JUCE processor.cpp

  pImpl->variables.global_enable = 0;
  ret = pImpl->headroom.headroom_init();
  return ret;  
}

TOVAL_ERROR TOVAL_Effect::TOVAL_Effect_set(uint16_t moduleID, uint16_t paramID, uint16_t datalength, void* data)
{
  TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;
  cout<<"Calling first module in TOVAL set"<<endl;

  /*
    Potential preset idea. Where the instance of pImpl is defined, you could have several Impl objects, one for each preset.
    This way you can pass Preset ID as an argument to the public functions, and then a switch case where you then call the do set,
    but it will be setting the private structure variables for that specific preset impl.

    Only try this addition of preset IDs to the public functions once library is working on 1 instance.
  */


  ret = pImpl->TOVAL_Effect_do_set(moduleID, paramID, datalength, data);
  //call the specific instance of the Internal Impl struct and its sub function.
  return ret;  
}

TOVAL_ERROR TOVAL_Effect::Impl::global_set(uint16_t paramID, uint16_t data_length, void* data)
{
  TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;

  switch(paramID)
  {
    case GlobalParams::ENABLE:
      if (data_length != sizeof(variables.global_enable))
      {
        ret = TOVAL_ERROR::SIZE_ERROR;
        cout << "enable size error!!!" << endl;
      }
      else if (data == nullptr)
      {
        ret = TOVAL_ERROR::NULL_POINTER_ERROR;
        cout << "nullptr error in enable" << endl;
      }
      else
      {
        uint32_t value = *static_cast<const uint32_t*>(data);
        variables.global_enable = value;
      }
      break;
  }
  return ret;
}

TOVAL_ERROR TOVAL_Effect::Impl::TOVAL_Effect_do_set(uint32_t moduleID, uint16_t paramID, uint16_t data_length, void* data)
{
    TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;

    // Check if this is the GLOBAL_BYPASS parameter
    //if (paramID == Modules::GLOBAL) {
        // Deserialize the data for GLOBAL_BYPASS
        //uint32_t global_enable_value = *static_cast<uint32_t*>(data);

        // Update the global_bypass flag based on the received value
        //variables.global_enable = (global_enable_value == 1) ? 1 : 0;

        // Update the global enable flag based on bypass setting
        //return ret;
    //}
    switch(moduleID)
    {
      case Modules::GLOBAL:
      std::cout << "Global module set called!!!" << std::endl;
        ret = global_set(paramID, data_length, (void*) data);
        break;

      case Modules::HEADROOM:
        std::cout << "Headroom module set called!!!" << std::endl;
        ret = headroom.headroom_set(paramID, data_length, (void*) data);
        break;
      
      default:
        cout << "Error: No Parameter recognised" << endl;
        ret = TOVAL_ERROR::PARAMID_ERROR;
        break;
    }
    return ret;
}

TOVAL_ERROR TOVAL_Effect::TOVAL_Effect_get()
{ 
  TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;
  cout<<"Calling first module in TOVAL get"<<endl;
  //ret = pImpl->softClip.softClip_get();
  
  return ret;
}

TOVAL_ERROR TOVAL_Effect::TOVAL_Effect_process(float **ppIn, float **ppOut, size_t nspc)
{
  TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;
  
  if(pImpl->variables.global_enable == 0)
  {
    for (uint32_t ch = 0; ch < pImpl->config.Out_num_channels; ch++)
    {
      if(ch > pImpl->config.In_num_channels)
      {
        memset(ppOut[ch], 0, static_cast<size_t>(nspc) * sizeof(float));
      }
      memcpy(ppOut[ch], ppIn[ch], static_cast<size_t>(nspc) * sizeof(float));
    }
  }
  else{
     ret = pImpl->headroom.headroom_process(ppIn, ppOut, nspc);
  }


  //if (pVariables->repeat_counter == 0);

  return ret;
}

void TOVAL_Effect::Impl::update_channel_config() {
    Modules first = MODULE_FIRST;
    Modules last = static_cast<Modules>(MODULE_COUNT - 1);
    config.In_num_channels = get_num_channels_for_module(first);
    config.Out_num_channels = get_num_channels_for_module(last);
}

TOVAL_ERROR TOVAL_Effect::set_config(size_t data_length, void *config_data)
{
    TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;
    if (data_length != sizeof(Impl::Config))
    {
        ret = TOVAL_ERROR::SIZE_ERROR;
    }

    else if (config_data == nullptr)
    {
        ret = TOVAL_ERROR::NULL_POINTER_ERROR;
    }
    else
    {
      Impl::Config values = *static_cast<const Impl::Config*>(config_data);
      pImpl->config = values;

      pImpl->update_channel_config();  // recalculate channel counts
    }
    return ret;
}

TOVAL_ERROR TOVAL_Effect::get_config(size_t data_length, void *config_data)
{
   TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;

    // Check if the provided data length matches the size of enable
    if (data_length != sizeof(Impl::Config))
    {
        ret = TOVAL_ERROR::SIZE_ERROR;
    }
    else if (config_data == nullptr)
    {
        ret = TOVAL_ERROR::NULL_POINTER_ERROR;
    }
    else
    {   
        pImpl->update_channel_config();  // ensure values are current
        *static_cast<Impl::Config*>(config_data) = pImpl->config;
    }

    return ret;
}

TOVAL_ERROR TOVAL_Effect::Impl::internalProcess()
{

  TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;
  //cout<<"Calling first module in TOVAL init"<<endl;
  //ret = pEQ.EQ_init();    // Only init because havent passed input yet
    // Private processing logic
  return ret;
}
