#ifndef AUDIOSIGNALPROCESSOR_H
#define AUDIOSIGNALPROCESSOR_H

#include "AudioSettingsStruct.h"
#include "AudioDataStruct.h"

class AudioSignalProcessor
{
public:
    AudioSignalProcessor();

    void SetNormalization(Audio::AudioSettingsStruct*);
    void Process(int, Audio::AudioSettingsStruct*);
    const Audio::AudioDataStruct& Data();

private:
    Audio::AudioDataStruct  data;
};

#endif // AUDIOSIGNALPROCESSOR_H
