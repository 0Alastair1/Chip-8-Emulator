#include <main.h>
#include <portaudio.h>
#include <math.h>

#define a_hz 440.0f
#define b_hz 880.0f
#define c_hz 1320.0f
#define d_hz 1760.0f
#define e_hz 2200.0f
#define f_hz 2640.0f
#define g_hz 3080.0f

#define leftOut (i * 2)
#define rightOut (i * 2 + 1)

const int sampleRate = 44100;
int bufferSize = 256;

PaStream *stream;

#define pi (double)3.14159265358979323846

void peError(PaError pa_Error);
static int callback(const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *data);

enum {
    mono = 1,
    stereo = 2
};

typedef enum {
    sineWave = 0,
    squareWave = 1,
    triangleWave = 2,
    sawtoothWave = 3,
} waveType;

typedef struct{

    float frequency_left;
    float frequency_right;
    float sampleRate;
    float amplitude;
    waveType wave;
    bool abort;
    bool mute;

} dataStruct;

dataStruct soundData = {
    a_hz,
    b_hz,
    sampleRate,
    1.0f,
    sineWave
};

void initAudio() 
{
    PaError pa_Error = Pa_Initialize();
    if (pa_Error != paNoError) {
        printf("PortAudio error: %s\n", Pa_GetErrorText(pa_Error));
        exit(1);
    }

    soundData.amplitude = 0.08f;
    soundData.wave = squareWave;

    pa_Error = Pa_OpenDefaultStream(&stream, 0, stereo, paFloat32, sampleRate, bufferSize, callback, &soundData);
    peError(pa_Error);

    return;
}

void startAudio()
{
    PaError pa_Error = Pa_StartStream(stream);
    soundData.abort = false;
    peError(pa_Error);
}

void stopAudio()
{
    /*PaError pa_Error = Pa_StopStream(stream);*/
    PaError pa_Error = Pa_AbortStream(stream);
    soundData.abort = true;   
    peError(pa_Error);
}

void muteAudio()
{
    soundData.mute = true;
}

void unmuteAudio()
{
    soundData.mute = false;
}


static int callback(const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags flags, void *data)
{
    float *out = (float *)output;
    dataStruct *d = (dataStruct *)data;

    float frequency_left = d->frequency_left;
    float frequency_right = d->frequency_right;
    float sampleRate = d->sampleRate;
    float amplitude = d->amplitude;
    
    if(d->mute)
    {
        for(int i = 0; i < frameCount * stereo; i++)
        {
            out[leftOut] = 0.0f;
            out[rightOut] = 0.0f;
        }
        return paContinue;
    }

    if(d->abort)
    {
        return paAbort;
    }

    
    for (int i = 0; i < frameCount; i++) {

        float dtime = (float)i / sampleRate;

        if(d->wave == sineWave)
        {
            out[leftOut] = amplitude * sin(frequency_left * (2 * pi) * dtime);
            out[rightOut] = amplitude * sin(frequency_right * (2 * pi) * dtime);

        }
        if(d->wave == squareWave)
        {
            float temp_left = amplitude * sin(frequency_left * (2 * pi) * dtime);
            float temp_right = amplitude * sin(frequency_right * (2 * pi) * dtime);

            if(temp_left > 0)
            {
                out[leftOut] = amplitude;
            }
            else
            {
                out[leftOut] = -amplitude;
            }
            if(temp_right > 0)
            {
                out[rightOut] = amplitude;
            }
            else
            {
                out[rightOut] = -amplitude;
            }
        }
        
    }

    return paContinue;
}

void peError(PaError pa_Error)
{
    if (pa_Error != paNoError) {
        printf("audio error: %s\n", Pa_GetErrorText(pa_Error));
        exit(1);
    }
}

void closeAudio() 
{
    Pa_Terminate();
}