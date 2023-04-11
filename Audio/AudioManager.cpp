#include "Audio/AudioManager.h"

#ifdef _WIN32
#include <stringapiset.h>
#endif

AudioManager* AudioManager::instance;

AudioManager* AudioManager::get()
{
    if(!instance)
    {
        instance = new AudioManager();
    }

    return instance;
}

AudioManager::AudioManager()
{
    InitAudioDeviceList();
}

AudioManager::~AudioManager()
{    
    /*-----------------------------*\
    | Remove all remaining clients  |
    \*-----------------------------*/
    active_clients.clear();

    /*--------------------------------*\
    | Stop all of the capture threads  |
    \*--------------------------------*/
    std::map<int, std::thread *>::iterator threads_it;

    for (threads_it = devices_capture_threads.begin(); threads_it != devices_capture_threads.end(); threads_it++)
    {
        int device_idx = threads_it->first;
        std::thread * thread = threads_it->second;
        ContinueCapture[device_idx] = false;
        thread->join();
        delete thread;
        CloseDevice(device_idx);
    }

    devices_capture_threads.clear();
}

std::vector<char *> AudioManager::GetAudioDevices()
{
    /*-----------------------------------------------------------------------*\
    | Give the requester a list of names of the devices we found during init  |
    \*-----------------------------------------------------------------------*/
    return known_audio_devices;
}

void AudioManager::Capture(int device_idx, float *buf)
{
    /*-----------------------------------------*\
    | memcopy device buffer into to passed ref  |
    \*-----------------------------------------*/
    if (devices_buffers.find(device_idx) != devices_buffers.end() )
    {
        memcpy(buf, devices_buffers[device_idx].data(), sizeof(float) * 512);
    }
}

void AudioManager::RegisterClient(int device_idx, void * client)
{
    /*-------------------------------------------------*\
    | Make sure that the client didn't try to register  |
    | For a non-existent device                         |
    \*-------------------------------------------------*/
    if(device_idx >= (int)known_audio_devices.size())
    {
        return;
    }

    /*-----------------------------------------------------------------*\
    | If the device doesn't have a client list already then create one  |
    \*-----------------------------------------------------------------*/
    if ( active_clients.find(device_idx) == active_clients.end() )
    {
        std::set<void *> clients_ptrs;
        active_clients[device_idx] = clients_ptrs;
    }

    /*--------------------------------*\
    | Add to the list active clients   |
    \*--------------------------------*/
    active_clients[device_idx].insert(client);

    /*--------------------------------------------------*\
    | If the Device doesn't already have a               |
    | Buffer then create an empty array for said buffer  |
    \*--------------------------------------------------*/
    if ( devices_buffers.find(device_idx) == devices_buffers.end() )
    {
        devices_buffers[device_idx] = std::array<float, 512>();
    }

    /*-------------------------------------------------------------*\
    | Start a capture thread for the device if it doesn't have one  |
    \*-------------------------------------------------------------*/
    if ( devices_capture_threads.find(device_idx) == devices_capture_threads.end() )
    {
        OpenDevice(device_idx);

        devices_capture_threads[device_idx] = new std::thread(&AudioManager::CaptureThreadFunction, this, device_idx);
    }
}

void AudioManager::UnRegisterClient(int device_idx, void * client)
{
    /*----------------------------*\
    | Check if device has clients  |
    \*----------------------------*/
    if ( active_clients.find(device_idx) != active_clients.end() )
    {
        active_clients[device_idx].erase(client);

        /*--------------------------------------------------*\
        | stop capture thread if no more clients for device  |
        \*--------------------------------------------------*/
        if(active_clients[device_idx].empty())
        {
            active_clients.erase(device_idx);

            ContinueCapture[device_idx] = false;
            std::thread* thread = devices_capture_threads[device_idx];
            thread->join();
            delete thread;

            devices_capture_threads.erase(device_idx);

            /*----------------------*\
            | delete unused buffers  |
            \*----------------------*/
            devices_buffers.erase(device_idx);

            /*-------------*\
            | Close device  |
            \*-------------*/
            CloseDevice(device_idx);
        }
    }
}

void AudioManager::InitAudioDeviceList()
{
    #ifdef _WIN32
    IMMDevice* pEndpoint;
    IPropertyStore* pProps;
    PROPVARIANT* varName;

    /*------------------------------------------------------*\
    | If using WASAPI, start WASAPI loopback capture device  |
    \*------------------------------------------------------*/
    CoInitializeEx(NULL, COINIT_MULTITHREADED);
    CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pMMDeviceEnumerator);

    for (int i = 0; i < (int)pMMDevices.size(); i++)
    {
        pMMDevices[i]->Release();
        if (i != 0)
        {
            delete known_audio_devices[i];
        }
    }

    pMMDevices.clear();
    known_audio_devices.clear();
    isCapture.clear();
    ContinueCapture.clear();

    /*------------------------*\
    | Enumerate audio outputs  |
    \*------------------------*/
    pMMDeviceEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pMMDeviceCollection);
    if (pMMDeviceCollection != NULL)
    {
        UINT count;
        pMMDeviceCollection->GetCount(&count);
        for (UINT i = 0; i < count; i++)
        {
            varName = new PROPVARIANT();

            /*-----------------------------*\
            | Query the item from the list  |
            \*-----------------------------*/
            pMMDeviceCollection->Item(i, &pEndpoint);

            /*---------------------------------------*\
            | Open property store for the given item  |
            \*---------------------------------------*/
            pEndpoint->OpenPropertyStore(STGM_READ, &pProps);

            /*------------------------------------*\
            | Get the friendly device name string  |
            \*------------------------------------*/
            pProps->GetValue(PKEY_Device_FriendlyName, varName);

            if (varName->pwszVal != NULL)
            {
                int len = WideCharToMultiByte(CP_UTF8, 0, varName->pwszVal, -1, nullptr, 0, nullptr, nullptr);
                char* new_device = new char[len + 11];
                WideCharToMultiByte(CP_UTF8, 0, varName->pwszVal, -1, new_device, len, nullptr, nullptr);
                strncat(new_device, " (Loopback)", len);
                known_audio_devices.push_back(new_device);
                pMMDevices.push_back(pEndpoint);
                isCapture.push_back(false);
                ContinueCapture.push_back(false);
            }
            delete varName;
            pProps->Release();
        }
    }
    pMMDeviceCollection->Release();

    /*-----------------------*\
    | Enumerate audio inputs  |
    \*-----------------------*/
    pMMDeviceEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &pMMDeviceCollection);
    if (pMMDeviceCollection != NULL)
    {
        UINT count;
        pMMDeviceCollection->GetCount(&count);
        for (UINT i = 0; i < count; i++)
        {
            varName = new PROPVARIANT();

            /*-----------------------------*\
            | Query the item from the list  |
            \*-----------------------------*/
            pMMDeviceCollection->Item(i, &pEndpoint);

            /*----------------------------------------*\
            | Open property store for the given item   |
            \*----------------------------------------*/
            pEndpoint->OpenPropertyStore(STGM_READ, &pProps);

            /*------------------------------------*\
            | Get the friendly device name string  |
            \*------------------------------------*/
            pProps->GetValue(PKEY_Device_FriendlyName, varName);

            if (varName->pwszVal != NULL)
            {
                int len = WideCharToMultiByte(CP_UTF8, 0, varName->pwszVal, -1, nullptr, 0, nullptr, nullptr);
                char* new_device = new char[len];
                WideCharToMultiByte(CP_UTF8, 0, varName->pwszVal, -1, new_device, len, nullptr, nullptr);
                known_audio_devices.push_back(new_device);
                pMMDevices.push_back(pEndpoint);
                isCapture.push_back(true);
                ContinueCapture.push_back(false);
            }
            delete varName;
            pProps->Release();
        }
    }
    pMMDeviceEnumerator->Release();
    #else

    /*----------------------------------------------------------------*\
    | If using OpenAL, start OpenAL capture on default capture device  |
    \*----------------------------------------------------------------*/

    ALCchar* devices;
    devices = (ALCchar *) alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);

    /*---------------------------------------------------------------------*\
    | Loop through detected capture devices and stop at the configured one  |
    \*---------------------------------------------------------------------*/
    char devicestring[512];
    char *devicep = devicestring;
    for(int i = 0; i < 512; i++)
    {
        *devicep = *devices;
        if(*devicep == '\0')
        {
            if(strlen(devicestring) > 0)
            {
                char* new_device = new char[strlen(devicestring) + 1];
                strcpy(new_device, devicestring);
                known_audio_devices.push_back(new_device);
                ContinueCapture.push_back(false);
            }


            i = 0;
            devices++;
            if(*devicep == '\0' && *devicestring == '\0')
            {
                break;
            }

            devicep = devicestring;
        }
        else
        {
            devices++;
            devicep++;
        }
    }
    #endif

}

void AudioManager::CaptureThreadFunction(int device_idx)
{
    printf("[OpenRGBEffectsPlugin] AUDIO: Thread %d started\n", device_idx);

    while(ContinueCapture[device_idx])
    {
        /*-----------------------------------------------*\
        | Track how long getting the audio buffer takes   |
        | so we can go at a solid 60 FPS                  |
        \*-----------------------------------------------*/
        auto start = std::chrono::steady_clock::now();

        #ifdef _WIN32

        unsigned int buffer_pos = 0;
        static float input_wave[512];

        unsigned int nextPacketSize = 1;
        unsigned int flags;

        IAudioCaptureClient * pAudioCaptureClient = active_audio_capture_clients[device_idx];

        while (nextPacketSize > 0 && ContinueCapture[device_idx])
        {
            float *buf;
            if (pAudioCaptureClient != NULL)
            {
                pAudioCaptureClient->GetBuffer((BYTE**)&buf, &nextPacketSize, (DWORD *)&flags, NULL, NULL);

                if (buf == NULL && nextPacketSize > 0)
                {
                    CloseDevice(device_idx);
                    printf("[OpenRGBEffectsPlugin] STOPPED Thread %d\n", device_idx);
                    return;
                }
                else
                {
                    for (unsigned int i = 0; i < nextPacketSize; i += 4)
                    {
                        for (int j = 0; j < 255; j++)
                        {
                            input_wave[2 * j] = input_wave[2 * (j + 1)];
                            input_wave[(2 * j) + 1] = input_wave[2 * j];
                        }

                        float avg_buf = (buf[i] + buf[i + 1] + buf[i + 2] + buf[i + 3]) / 4;
                        input_wave[510] = avg_buf * 2.0f;
                        input_wave[511] = input_wave[510];
                    }

                    buffer_pos += nextPacketSize / 4;
                    pAudioCaptureClient->ReleaseBuffer(nextPacketSize);
                }
            }
        }
        memcpy(devices_buffers[device_idx].data(), input_wave, sizeof(input_wave));

        #else
        int samples;

        ALCdevice * device = active_acl_devices[device_idx];

        if(device != nullptr)
        {
            do
            {
                alcGetIntegerv(device, ALC_CAPTURE_SAMPLES, 1, &samples);
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            } while ((samples < 512) && ContinueCapture[device_idx]);
        }

        unsigned char buffer[256];

        /*--------------------------*\
        | Capture 256 audio samples  |
        \*--------------------------*/
        alcCaptureSamples(device, (ALCvoid *)buffer, 256);

        /*----------------------*\
        | copy to device buffer  |
        \*----------------------*/
        for (int i = 0; i < 512; i++)
        {
            devices_buffers[device_idx][i] = buffer[i / 2];
        }
        #endif

        /*--------------------*\
        | capturing global FPS |
        \*--------------------*/
        int took = (int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
        int delta = delay - took;
        std::this_thread::sleep_for(std::chrono::milliseconds(delta > 0 ? delta:1));
    }

    printf("[OpenRGBEffectsPlugin] AUDIO: Thread %d stopped\n", device_idx);

}

void AudioManager::OpenDevice(int device_idx)
{
    printf("[OpenRGBEffectsPlugin] AUDIO: Opening device %d\n" , device_idx);
    ContinueCapture[device_idx] = true;

    #ifdef _WIN32

    /*------------*\
    | open device  |
    \*------------*/
    if (device_idx < known_audio_devices.size())
    {
        IAudioCaptureClient * pAudioCaptureClient;
        IAudioClient * pAudioClient;

        IMMDevice* pMMDevice = pMMDevices[device_idx];
        pMMDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&pAudioClient);

        pAudioClient->GetMixFormat(&waveformat);

        if (isCapture[device_idx])
        {
            pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 0, 0, waveformat, 0);
        }
        else
        {
            pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, 0, 0, waveformat, 0);
        }

        pAudioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&pAudioCaptureClient);
        pAudioClient->Start();

        active_audio_clients[device_idx] = pAudioClient;
        active_audio_capture_clients[device_idx] = pAudioCaptureClient;
    }
    #else

    /*----------------*\
    | Open the device  |
    \*----------------*/

    if (active_acl_devices.find(device_idx) == active_acl_devices.end() )
    {
        active_acl_devices[device_idx] = alcCaptureOpenDevice(known_audio_devices[device_idx], 10000, AL_FORMAT_MONO8, 2048);
        alcCaptureStart(active_acl_devices[device_idx]);
    }

    #endif

}

void AudioManager::CloseDevice(int device_idx)
{
    printf("[OpenRGBEffectsPlugin] AUDIO: Closing device %d\n" , device_idx);
    ContinueCapture[device_idx] = false;

    #ifdef _WIN32
    if (active_audio_clients.find(device_idx) != active_audio_clients.end() )
    {
        active_audio_clients[device_idx]->Stop();
        active_audio_clients[device_idx]->Release();

        active_audio_clients.erase(device_idx);
        active_audio_capture_clients.erase(device_idx);
    }
    #else
    if (active_acl_devices.find(device_idx) != active_acl_devices.end() )
    {
        alcCaptureCloseDevice(active_acl_devices[device_idx]);
        alcCaptureStop(active_acl_devices[device_idx]);
        active_acl_devices.erase(device_idx);
    }
    #endif
}
