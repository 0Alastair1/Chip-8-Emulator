 #ifdef __cplusplus
 #define EXTERNC extern "C"
 #else
 #define EXTERNC
 #endif

EXTERNC void initAudio();
EXTERNC void startAudio();
EXTERNC void closeAudio();
EXTERNC void muteAudio();
EXTERNC void unmuteAudio();
EXTERNC void changeAudioData(uint16_t ex);
EXTERNC void updateAudioPattern(uint8_t** audioPattern);