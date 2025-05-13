#include <iostream>
#include <vector>
#include <filesystem>
#include <sndfile.h>  // libsndfile for WAV handling
#include "TOVALaudio.h"  // Your module's header file
#include "TOVAL_Effect.h"

#define INPUT_FOLDER "./test_wavs"  // Folder containing WAV files
#define OUTPUT_FILE "output.wav"    // Output file

namespace fs = std::filesystem;

class Tonal_Valley_test
{
private:

    std::vector<float> inputBuffer;
    std::vector<float> outputBuffer;

    std::vector<std::vector<float>> deinterleavedInput;
    std::vector<std::vector<float>> deinterleavedOutput;
    std::vector<std::vector<float>> deinterleavedRef;


    std::vector<float*> ppIn;
    std::vector<float*> ppOut;
    std::vector<float*> ppRef;



    size_t nspc = 1024;
    size_t chunkSize;
    uint16_t In_num_channels;
    uint16_t Out_num_channels;

    int numFrames;

    struct Test_config
    {
        uint16_t In_num_channels;
        uint16_t Out_num_channels;
    }test_config;

    struct WavHeader
        {
            uint8_t  RIFF[4];               // "RIFF"
            uint32_t ChunkSize;             // Overall file size - 8 bytes
            uint8_t  WAVE[4];               // "WAVE"
            uint8_t  fmt[4];                // "fmt "
            uint32_t SubChunk1Size;         // Size of fmt chunk
            uint16_t AudioFormat;           // 1 = PCM
            uint16_t NumChannels;           // 1 = mono, 2 = stereo
            uint32_t SampleRate;            // Sampling rate
            uint32_t ByteRate;              // SampleRate * NumChannels * BitsPerSample/8
            uint16_t BlockAlign;            // NumChannels * BitsPerSample/8
            uint16_t BitsPerSample;         // Bits per sample
            uint8_t  SubChunk2ID[4];        // "data"
            uint32_t SubChunk2Size;         // Data size
        };

public:

    WavHeader inputWavHeader; // Stores input WAV file header info
    WavHeader outputWavHeader; // Stores output WAV file header info

    TOVAL_Effect tonal_valley_test;
    
    Tonal_Valley_test();
    ~Tonal_Valley_test();

    TOVAL_ERROR loadWav(const std::string &filename);
    void printWavHeader(const WavHeader& header);
    TOVAL_ERROR prepareAudio();
    TOVAL_ERROR processAudio();
    TOVAL_ERROR saveWav(const std::string &filename);

    TOVAL_ERROR deinterleave(const std::vector<float>& interleaved, std::vector<std::vector<float>>& channels, int numChannels);

};
