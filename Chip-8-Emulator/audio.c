#include <main.h>
#include <portaudio.h>
#include <math.h>
#include <samplerate.h>

/* check me - should use C++ atomics?, stdatomics?, patomics? which are lock free?*/
#include <stdatomic.h>
#include <patomic.h>

#define a_hz 440.0f
#define b_hz 880.0f
#define c_hz 1320.0f
#define d_hz 1760.0f
#define e_hz 2200.0f
#define f_hz 2640.0f
#define g_hz 3080.0f

//#define leftOut (i * 2)
//#define rightOut (i * 2 + 1)

#define leftOut (i)
#define rightOut (i + 1)

const int sampleRate = 44100;
int bufferSize = 128; /* xomode uses 128 samples (16bytes) per frame */

PaStream *stream;

float* tempOutLeft;
float* tempOutRight;

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

    _Atomic float frequency_left;
    _Atomic float frequency_right;
    _Atomic float sampleRate;
    _Atomic float amplitude;
    _Atomic waveType wave;
    _Atomic bool abort;
    _Atomic bool mute;
    _Atomic bool xoMode;
    _Atomic float audioPPR;
    Uint8** audioPattern;

} dataStruct;

dataStruct soundData = {
    a_hz,
    b_hz,
    sampleRate,
    1.0f,
    sineWave,
    false,
    false,
    false,
    0.0f,
    NULL
};

void initAudio() 
{
    PaError pa_Error = Pa_Initialize();
    if (pa_Error != paNoError) {
        printf("PortAudio error: %s\n", Pa_GetErrorText(pa_Error));
        exit(1);
    }

    if(p_atomic_is_lock_free() == FALSE)
    { /* does this apply to c standard atomics? */
        printf("compiler doesn't support lock free atomics\n");
        exit(1);
    }

    /*
    tempOutLeft = (float*)malloc(sizeof(float) * bufferSize);
    tempOutRight = (float*)malloc(sizeof(float) * bufferSize);
    */
   /* check me */
    tempOutLeft = (float*)alloca(sizeof(float) * bufferSize);
    tempOutRight = (float*)alloca(sizeof(float) * bufferSize);

    soundData.xoMode = false;
    soundData.amplitude = 0.08f;
    soundData.wave = squareWave;

    pa_Error = Pa_OpenDefaultStream(&stream, 0, stereo, paFloat32, sampleRate, bufferSize, callback, &soundData);
    peError(pa_Error);
    

    return;
}

static void updateAudioOutput()
{
    int i;
    for(i =0; i < 128; i++)
    {
        if(**soundData.audioPattern >> i & 1) /* xo uses square wave */
        {
            tempOutLeft[i] = soundData.amplitude;
            tempOutRight[i] = soundData.amplitude;
        }
        else
        {
            tempOutLeft[i] = 0.0f;
            tempOutRight[i] = 0.0f;
        }
    }
    /* recalculate samplerate conversion */
    /* recalculate samplerate from audioPPR to sampleRate using wave from tempOutLeft and tempOutRight */

    SRC_DATA srcData;
    srcData.data_in = tempOutLeft;
    srcData.data_out = tempOutLeft;
    srcData.input_frames = bufferSize;
    srcData.output_frames = bufferSize;
    srcData.src_ratio = sampleRate / soundData.audioPPR;

    /* using src_simple as audiopattern is the full audio buffer */
    int error = src_simple(&srcData, SRC_SINC_MEDIUM_QUALITY, SRC_LINEAR);
    if(error)
    {
        printf("error: %s\n", src_strerror(error));
        exit(1);
    }

    srcData.data_in = tempOutRight;
    srcData.data_out = tempOutRight;
    srcData.input_frames = bufferSize;
    srcData.output_frames = bufferSize;
    srcData.src_ratio = sampleRate / soundData.audioPPR;

    error = src_simple(&srcData, SRC_SINC_MEDIUM_QUALITY, SRC_LINEAR);
    if(error)
    {
        printf("error: %s\n", src_strerror(error));
        exit(1);
    }

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
    printf("mute\n");
    soundData.mute = true;
}

void unmuteAudio()
{
    soundData.mute = false;
}

void changeAudioData(Uint16 ex)
{
    bool mutePush = soundData.mute;
    if(soundData.mute == false)
    {
        muteAudio();
    }

    float audioPPR = (float)4000 * (float)(2^((ex-64)/48)); /* xo pitchhz/samplerate/playbackrate */
    soundData.audioPPR = audioPPR;
    updateAudioOutput();

    soundData.mute = mutePush;
}

void updateAudioPattern(Uint8** audioPattern)
{
    bool mutePush = soundData.mute;
    if(soundData.mute == false)
    {
        muteAudio();
    }

    soundData.audioPattern = audioPattern;
    updateAudioOutput();

    soundData.mute = mutePush;
}

void setXoMode(bool xomode)
{
    soundData.xoMode = xomode;
}

static int callback(const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags flags, void *data)
{
    float *out = (float *)output;
    dataStruct *d = (dataStruct *)data;

    float frequency_left = d->frequency_left;
    float frequency_right = d->frequency_right;
    float sampleRate = d->sampleRate;
    float amplitude = d->amplitude;
    waveType wave = d->wave;
    bool abort = d->abort;
    bool mute = d->mute;
    bool xoMode = d->xoMode;
    float audioPPR = d->audioPPR;
    
    if(mute)
    {
        int i;
        for(i = 0; i < frameCount * stereo; i++)
        {
            out[leftOut] = 0.0f;
            out[rightOut] = 0.0f;
        }
        return paContinue;
    }

    if(abort)
    {
        return paAbort;
    }

    if(!xoMode)
    {
        /* construct and output waveform using desired frequency, amplitude and samplerate */
        int i;
        for (i = 0; i < frameCount; i++) {

            float dtime = (float)i / sampleRate;

            float angleLeft = 2.0f * pi * frequency_left;
            float angleRight = 2.0f * pi * frequency_right;


            if(wave == sineWave)
            {
                out[leftOut] = amplitude * sin(angleLeft * dtime);
                out[rightOut] = amplitude * sin(angleRight * dtime);

            }
            if(wave == squareWave)
            {
                float temp_left = amplitude * sin(angleLeft * dtime);
                float temp_right = amplitude * sin(angleRight * dtime);

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
    }
    else{
        /* XO mode audio */

        if(audioPPR == 0)
        {
            return paContinue;
        }

        /* output the waveform contained in audiopattern 
            at the samplerate calculated from the pitch register converted to our samplerate */
        Uint8 i;
        for (i = 0; i < 128; i++)
        {
            out[leftOut] = tempOutLeft[i];
            out[rightOut] = tempOutRight[i];
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