#include <main.h>
#include <portaudio.h>

void initAudio() 
{
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));
        return 1;
    }
    return 0;
}

void closeAudio() 
{
    Pa_Terminate();
}