#ifndef TOVAL_EFFECT_H
#define TOVAL_EFFECT_H

#include "TOVALaudio.h"

class TOVAL_Effect {
public:
    TOVAL_Effect();
    ~TOVAL_Effect();

    // Public methods
    TOVAL_ERROR TOVAL_Effect_init();
    TOVAL_ERROR TOVAL_Effect_set(uint16_t moduleID, uint16_t paramID, uint16_t datalength, void* data);
    TOVAL_ERROR TOVAL_Effect_get();
    TOVAL_ERROR TOVAL_Effect_process(float **ppIn, float **ppOut, size_t nspc);
    
    TOVAL_ERROR get_config(size_t data_length, void *config_data);
    TOVAL_ERROR set_config(size_t data_length, void *config_data);

private:
    struct Impl;
    Impl* pImpl;  // Pointer to the private implementation
    
    // Maybe remove
    struct Variables;
    Variables* pVariables;
    //Test
};

#endif // TOVAL_EFFECT_H
