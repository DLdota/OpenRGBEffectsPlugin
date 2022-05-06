#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <array>
#include <vector>
#include <set>
#include <map>

/*---------------------------------------------------------*\
| Audio Library Includes                                    |
\*---------------------------------------------------------*/
#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#include <initguid.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <functiondiscoverykeys_devpkey.h>
#endif

#ifdef __linux__
#include <AL/al.h>
#include <AL/alc.h>
#endif

#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#endif

#ifdef __FreeBSD__
#include <AL/al.h>
#include <AL/alc.h>
#endif

class AudioManager
{

public:
    static AudioManager*     get();

    /*-------------------------------------*\
    | Passes list of devices to requester   |
    \*-------------------------------------*/
    std::vector<char *>      GetAudioDevices();

    /*-----------------------------------*\
    | Client Handling and buffer passing  |
    \*-----------------------------------*/
    void                     Capture(int device_idx, float *buf);
    void                     RegisterClient(int device_idx, void * client);
    void                     UnRegisterClient(int device_idx, void * client);

private:
    AudioManager();
    ~AudioManager();

    void InitAudioDeviceList(); // Couldn't this just be the constructor?

    // static instance, singleton
    static AudioManager*                    instance;

    // List of discovered audio devices
    std::vector<char *>                      known_audio_devices;

    /*-----------------------------*\
    | one device to many clients    |
    | one device to one thread      |
    | one device to one buffer      |
    \*-----------------------------*/
    std::map<int, std::set<void *>>          active_clients;
    std::map<int, std::thread *>             devices_capture_threads;
    std::map<int, std::array<float, 512> >   devices_buffers;
    int                                      delay = 1000 / 60;

    std::vector<bool>                        ContinueCapture;
#ifdef WIN32
    IMMDeviceEnumerator                    *pMMDeviceEnumerator;
    std::vector<IMMDevice *>               pMMDevices;
    IMMDeviceCollection                    *pMMDeviceCollection;

    std::vector<bool>                      isCapture;

    // remark: not sure about this, WAVEFORMATEX related to one device?
    WAVEFORMATEX                             *waveformat;

    std::map<int, IAudioClient *>            active_audio_clients;
    std::map<int, IAudioCaptureClient *>     active_audio_capture_clients;

#else
    std::map<int, ALCdevice *>                active_acl_devices;

#endif

    /*------------------*\
    | Capture Handling   |
    \*------------------*/
    void CaptureThreadFunction(int device_idx);

    /*--------------------------*\
    | Device Opening and Closing |
    \*--------------------------*/
    void OpenDevice(int device_idx);
    void CloseDevice(int device_idx);


};


#endif // AUDIOMANAGER_H
