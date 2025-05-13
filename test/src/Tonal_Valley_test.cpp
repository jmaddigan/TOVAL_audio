#include "Tonal_Valley_test.h"
#include "JsonParams.h"               // Include the header for JSON parameter functions
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>

// Constructor
Tonal_Valley_test::Tonal_Valley_test() {}

// Destructor
Tonal_Valley_test::~Tonal_Valley_test() {}

// Load WAV File and Store Header
TOVAL_ERROR Tonal_Valley_test::loadWav(const std::string &filename)
{
    TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;
    SF_INFO sfinfo;
    std::cout << "Loading " << filename.c_str() << "...\n" << std::endl;
    SNDFILE *infile = sf_open(filename.c_str(), SFM_READ, &sfinfo);

    if (!infile)
    {
        std::cerr << "Error: Could not open input WAV file: " << filename << std::endl;
        ret = TOVAL_ERROR::INPUT_WAV_ERROR;
    }

    if(ret == TOVAL_ERROR::NO_ERROR)
    {
        // Store header information for the input WAV file
        memcpy(inputWavHeader.RIFF, "RIFF", 4);
        memcpy(inputWavHeader.WAVE, "WAVE", 4);
        memcpy(inputWavHeader.fmt, "fmt ", 4);
        memcpy(inputWavHeader.SubChunk2ID, "data", 4);
        
        inputWavHeader.ChunkSize = sfinfo.frames * sfinfo.channels * sizeof(float) + 36;
        inputWavHeader.SubChunk1Size = 16;  // PCM
        inputWavHeader.AudioFormat = 1;  // PCM format
        inputWavHeader.NumChannels = sfinfo.channels;
        inputWavHeader.SampleRate = sfinfo.samplerate;
        inputWavHeader.BitsPerSample = 16;  // Assuming 16-bit WAV
        inputWavHeader.ByteRate = sfinfo.samplerate * sfinfo.channels * inputWavHeader.BitsPerSample / 8;
        inputWavHeader.BlockAlign = sfinfo.channels * inputWavHeader.BitsPerSample / 8;
        inputWavHeader.SubChunk2Size = sfinfo.frames * sfinfo.channels * sizeof(float);

        numFrames = sfinfo.frames;
        int numChannels = sfinfo.channels;

        std::cout << "Allocating buffer for input data: " << numFrames * numChannels << " floats" << std::endl;
        inputBuffer.resize(numFrames * numChannels);

        std::cout << "Reading WAV file data..." << std::endl;
        sf_read_float(infile, inputBuffer.data(), numFrames * numChannels);
        std::cout << "Finished reading WAV file data." << std::endl;
        sf_close(infile);

        //std::cout << "Allocating buffer for output data: " << numFrames << " frames" << std::endl;
        //outputBuffer.resize(numFrames); // Allocate space for processed data
        // need to accomodate for additional channels after processing
    }

    return ret;
}

// Print WAV Header Information
void Tonal_Valley_test::printWavHeader(const WavHeader& header)
{
    std::cout << "---------------- WAV HEADER INFO ----------------" << std::endl;
    std::cout << "RIFF Header:            " << std::string((char*)header.RIFF, 4) << std::endl;
    std::cout << "WAVE Header:            " << std::string((char*)header.WAVE, 4) << std::endl;
    std::cout << "Format:                 " << std::string((char*)header.fmt, 4) << std::endl;
    std::cout << "Audio Format:           " << header.AudioFormat << std::endl;
    std::cout << "Channels:               " << header.NumChannels << std::endl;
    std::cout << "Sample Rate:            " << header.SampleRate << " Hz" << std::endl;
    std::cout << "Byte Rate:              " << header.ByteRate << " bytes/sec" << std::endl;
    std::cout << "Block Align:            " << header.BlockAlign << std::endl;
    std::cout << "Bits per Sample:        " << header.BitsPerSample << " bits" << std::endl;
    std::cout << "Data Chunk ID:          " << std::string((char*)header.SubChunk2ID, 4) << std::endl;
    std::cout << "Data Size:              " << header.SubChunk2Size << " bytes" << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
}

// Save WAV File with the Same Header Structure
TOVAL_ERROR Tonal_Valley_test::saveWav(const std::string &filename)
{
    TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;

    Out_num_channels = ppOut.size();

    std::cout << "Allocating buffer for output data: " << numFrames * Out_num_channels << " frames" << std::endl;
    outputBuffer.resize(numFrames * Out_num_channels); // Allocate space for processed data

    SF_INFO sfinfo = {};
    sfinfo.samplerate = inputWavHeader.SampleRate;  // Use the input sample rate
    sfinfo.channels = Out_num_channels;
    sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    sfinfo.frames = outputBuffer.size() / Out_num_channels;
    SNDFILE *outfile = sf_open(filename.c_str(), SFM_WRITE, &sfinfo);
    if (!outfile)
    {
        std::cerr << "Error: Could not save output WAV file: " << filename << std::endl;
        ret = TOVAL_ERROR::INPUT_WAV_ERROR;;
    }

    if(ret == TOVAL_ERROR::NO_ERROR)
    {
        sf_write_float(outfile, outputBuffer.data(), outputBuffer.size());
        sf_close(outfile);
    }

    return ret;
}

TOVAL_ERROR Tonal_Valley_test::prepareAudio()
{
    TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;

    std::cout << "Preparing audio for test case " << std::endl;
    std::cout << "Number of sampes per channel for test = " << nspc << std::endl;

    ret = tonal_valley_test.get_config(sizeof(test_config), &test_config);  // Fill config data from effect data

    if(ret != TOVAL_ERROR::NO_ERROR)
    {
        std::cout<< "Config Error: Error code == " << static_cast<int>(ret) << std::endl;
        return ret;
    }

    // Validate channel count
    if(inputWavHeader.NumChannels != test_config.In_num_channels)
    {
        std::cout << "ERROR: Input wav file does not have required number of channels" << std::endl;
        std::cout << "Input Wav file number of channels = " << inputWavHeader.NumChannels << std::endl;
        std::cout << "Required number Input channels = " << test_config.In_num_channels << std::endl;
        ret = TOVAL_ERROR::CONFIG_ERROR;
        return ret;
    }
    else{
        std::cout << "Config number Input channels = " << test_config.In_num_channels << std::endl;
        std::cout << "Config number Output channels = " << test_config.Out_num_channels << std::endl;
    }

    // Calculate number of frames (same across input/output)
    int numFrames = static_cast<int>(inputBuffer.size()) / test_config.In_num_channels;

    std::cout << "Number frames = " << numFrames << std::endl;

    // Resize and deinterleave input buffer
    deinterleavedInput.resize(test_config.In_num_channels);
    for (int ch = 0; ch < test_config.In_num_channels; ++ch)
    {
        deinterleavedInput[ch].resize(numFrames);
    }

    ret = deinterleave(inputBuffer, deinterleavedInput, test_config.In_num_channels);

    if(ret != TOVAL_ERROR::NO_ERROR)
    {
        std::cout<< "Error in deinterleaving Input: Error code == " << static_cast<int>(ret) << std::endl;
        return ret;
    }

    // Resize and zero-init deinterleaved output buffers (allowing upmixing)
    deinterleavedOutput.resize(test_config.Out_num_channels);
    for (int ch = 0; ch < test_config.Out_num_channels; ++ch)
    {
        deinterleavedOutput[ch].resize(numFrames, 0.0f);
    }

    // Optional: resize reference buffer (not used here)
    deinterleavedRef.resize(test_config.Out_num_channels);
    for (int ch = 0; ch < test_config.Out_num_channels; ++ch)
    {
        deinterleavedRef[ch].resize(numFrames, 0.0f);  // Fill if needed
    }


    // Assign float** pointers for module processing
    ppIn.resize(test_config.In_num_channels);
    ppOut.resize(test_config.Out_num_channels);
    ppRef.resize(test_config.Out_num_channels);

    float **pIn = ppIn.data();
    float **pOut = ppOut.data();
    float **pRef = ppRef.data();

    for (int ch = 0; ch < test_config.In_num_channels; ++ch)
        ppIn[ch] = deinterleavedInput[ch].data();

    for (int ch = 0; ch < test_config.Out_num_channels; ++ch)
        ppOut[ch] = deinterleavedOutput[ch].data();

    for (int ch = 0; ch < test_config.Out_num_channels; ++ch)
        ppRef[ch] = deinterleavedRef[ch].data();  // if used

    // Pin/pOut is now assigned
    
    //In_num_channels = inputWavHeader.NumChannels;
    chunkSize = nspc;

    // TO do, change set config function to pass nspc data to the modules (can be a different structure, as you dont need to send channel numbers back)
    
    ret = tonal_valley_test.TOVAL_Effect_init();
    if(ret != TOVAL_ERROR::NO_ERROR)
    {
        std::cout<< "Init error occured!" << std::endl;
        return ret;
    }
    return ret;
}

TOVAL_ERROR Tonal_Valley_test::deinterleave(const std::vector<float>& interleaved, std::vector<std::vector<float>>& channels, int numChannels)
{
    TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;
    if (numChannels <= 0)
    {
        std::cout << "Error. Channels are less than 1 " << std::endl;
        ret = TOVAL_ERROR::CONFIG_ERROR;
        return ret;
    }

    int numFrames = static_cast<int>(interleaved.size()) / numChannels;
    channels.resize(numChannels);

    for (int ch = 0; ch < numChannels; ++ch)
    {
        channels[ch].resize(numFrames);
    }

    for (int frame = 0; frame < numFrames; ++frame)
    {
        for (int ch = 0; ch < numChannels; ++ch)
        {
            channels[ch][frame] = interleaved[frame * numChannels + ch];
        }
    }
    return ret;
}


// Dummy Audio Processing Function (Replace with Real Processing)
TOVAL_ERROR Tonal_Valley_test::processAudio()
{
    TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;

    std::cout << "Starting processAudio function..." << std::endl;

    const size_t totalInputFrames = deinterleavedInput[0].size();     // e.g. 1000 frames
    const size_t totalOutputFrames = deinterleavedOutput[0].size();   // e.g. 1000 frames, or more if upmixing
    const size_t totalChunks = (totalInputFrames + chunkSize - 1) / chunkSize;
    
    std::cout << "Number of input frames = " << totalInputFrames << std::endl;
    std::cout << "Number of output frames = " << totalInputFrames << std::endl;
    std::cout << "Total number of Chunks = " << totalChunks << std::endl;

    std::vector<std::vector<float>> inputChunks(test_config.In_num_channels, std::vector<float>(chunkSize));
    std::vector<std::vector<float>> outputChunks(test_config.Out_num_channels, std::vector<float>(chunkSize));

    for (size_t chunkIndex = 0; chunkIndex < totalChunks; ++chunkIndex)
    {
        size_t chunkStart = chunkIndex * chunkSize;
        size_t actualChunkSize = std::min(chunkSize, totalInputFrames - chunkStart);

        // Prepare chunk pointers
        for (int ch = 0; ch < test_config.In_num_channels; ++ch)
        {
            std::copy_n(deinterleavedInput[ch].begin() + chunkStart, actualChunkSize, inputChunks[ch].begin());
            ppIn[ch] = inputChunks[ch].data();
        }

        for (int ch = 0; ch < test_config.Out_num_channels; ++ch)
        {
            std::fill(outputChunks[ch].begin(), outputChunks[ch].end(), 0.0f);  // Optional: zero before processing
            ppOut[ch] = outputChunks[ch].data();
        }
        // Process
        if(chunkIndex == 0)
        {
            std::cout << "Starting TOVAL Process call..." << std::endl;
        }
        ret = tonal_valley_test.TOVAL_Effect_process(ppIn.data(), ppOut.data(), static_cast<int>(actualChunkSize));
        if (ret != TOVAL_ERROR::NO_ERROR)
        {
            std::cerr << "Processing failed on chunk " << chunkIndex << std::endl;
            return ret;
        }
        // Copy output
        for (int ch = 0; ch < test_config.Out_num_channels; ++ch)
        {
            std::copy_n(outputChunks[ch].begin(), actualChunkSize, deinterleavedOutput[ch].begin() + chunkStart);
        }
    }
    
    std::cout << "TOVAL process complete"<< std::endl;

    // Interleave output
    outputBuffer.resize(totalOutputFrames * test_config.Out_num_channels);
    for (size_t frame = 0; frame < totalOutputFrames; ++frame)
    {
        for (int ch = 0; ch < test_config.Out_num_channels; ++ch)
        {
            outputBuffer[frame * test_config.Out_num_channels + ch] = deinterleavedOutput[ch][frame];
        }
    }

    return TOVAL_ERROR::NO_ERROR;

}


int main(int argc, char *argv[])
{
    TOVAL_ERROR ret = TOVAL_ERROR::NO_ERROR;

    // Instantiate the test unit
    Tonal_Valley_test unit_test;

    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <test_case_directory>" << std::endl;
        return 1;
    }

    // Get test case directory (example: "../test/test_cases/02_default")
    std::string testCaseDir = argv[1];
    std::string outputDir = argv[2];


    // Extract a friendly test case name from the directory path
    std::string testCaseName = std::filesystem::path(testCaseDir).filename().string();
    std::cout << "Initiating test case " << testCaseName << "...\n" << std::endl;

    std::string inputWavPath;

    // Find the first WAV file in the test case directory
    for (const auto &entry : fs::directory_iterator(testCaseDir))
    {
        if (entry.path().extension() == ".wav")
        {
            inputWavPath = entry.path().string();
            break;
        }
    }

    if (inputWavPath.empty())
    {
        std::cerr << "Error: No WAV file found in " << testCaseDir << std::endl;
        return 1;
    }

        // Load WAV file
    ret = unit_test.loadWav(inputWavPath);
    if (ret != TOVAL_ERROR::NO_ERROR)
    {
        return 1;
    }

    // Print header information for verification
    unit_test.printWavHeader(unit_test.inputWavHeader);  // Print header before processing

    // Prepare audio (which may verify configuration etc.)
    ret = unit_test.prepareAudio();
    if (ret != TOVAL_ERROR::NO_ERROR)
    {
        std::cout << "Error in Prepare audio function. Error code: " << static_cast<int>(ret) << std::endl;
        return 1;
    }
    else{
        std::cout << "PrepareAudio complete" << std::endl;
    }

    // *** NEW: Load the JSON parameters from the test case directory ***
    std::string paramsFile = testCaseDir + "/params.json";
    if (fs::exists(paramsFile))
    {
        std::ifstream jsonFile(paramsFile);
        if (!jsonFile.is_open())
        {
            std::cerr << "Error: Unable to open JSON file: " << paramsFile << std::endl;
            return 1;
        }
        nlohmann::json jsonParams;
        try {
            jsonFile >> jsonParams;
        } catch (const std::exception& e) {
            std::cerr << "Error parsing JSON: " << e.what() << std::endl;
            return 1;
        }
        
        // Apply parameters to the effect instance using the provided generic JSON loader.
        ret = load_and_set_json_params(jsonParams, unit_test.tonal_valley_test);
        if (ret != TOVAL_ERROR::NO_ERROR)
        {
            std::cerr << "Error applying JSON parameters (code " << static_cast<int>(ret) << ")" << std::endl;
            return 1;
        }
        else {
            std::cout << "JSON parameters loaded successfully." << std::endl;
        }
    }
    else
    {
        std::cout << "No JSON parameter file found in " << testCaseDir << ". Using default parameters." << std::endl;
    }
    // *** End JSON loading ***

    // Process Audio
    ret = unit_test.processAudio();
    if (ret != TOVAL_ERROR::NO_ERROR)
    {
        std::cout << "Error in Process audio function. Error code: " << static_cast<int>(ret) << std::endl;
        return 1;
    }
     else{
        std::cout << "ProcessAudio complete" << std::endl;
    }

    // Save output WAV file
    std::string outputWavPath = outputDir + "/" + OUTPUT_FILE;
    ret = unit_test.saveWav(outputWavPath);
    if (ret != TOVAL_ERROR::NO_ERROR)
    {
        return 1;
    }

    std::cout << "Processing complete. Output saved to: " << outputWavPath << std::endl;
    return 0;
}