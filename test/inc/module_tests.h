#ifndef SOFTCLIP_TEST_H
#define SOFTCLIP_TEST_H

#include <iostream>
#include "TOVALaudio.h"

/*
    Can look into using inheritance for all module test classes
    parent class : Global test
    child class : Module test
*/


class SoftClipTest {

    public:

    int test_main();

    private:
    SoftClip test_softClip;
};

#endif