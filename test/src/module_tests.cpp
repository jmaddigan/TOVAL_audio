#include "module_tests.h"

int SoftClipTest::test_main() {
    
    TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;

    ret = test_softClip.softClip_init();
    if (ret != TOVAL_ERROR::NO_ERROR)
    {
        std::cerr << "Error occurred at Soft Clip init: " << static_cast<int>(ret) << std::endl;
    }

    //ret = test_softClip.softClip_set();
    if (ret != TOVAL_ERROR::NO_ERROR) {
        // Handle the error accordingly, e.g., print an error message
        std::cerr << "Error occurred at Soft Clip set: " << static_cast<int>(ret) << std::endl;
        goto end;
    }

    //ret = test_softClip.softClip_get();
    if (ret != TOVAL_ERROR::NO_ERROR) {
        // Handle the error accordingly, e.g., print an error message
        std::cerr << "Error occurred at Soft Clip get: " << static_cast<int>(ret) << std::endl;
        goto end;
    }

    ret = test_softClip.softClip_process();
    if (ret != TOVAL_ERROR::NO_ERROR) {
        // Handle the error accordingly, e.g., print an error message
        std::cerr << "Error occurred at Soft Clip process: " << static_cast<int>(ret) << std::endl;
        goto end;
    }

end:
    return static_cast<int>(ret); // Return the error code as an int
}

int main() {
    SoftClipTest moduleTest;
    int ret = moduleTest.test_main();
    return ret;  // Call the class method main()
}
