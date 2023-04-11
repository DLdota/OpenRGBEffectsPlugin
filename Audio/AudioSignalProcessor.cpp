#include "Audio/AudioSignalProcessor.h"
#include "Audio/AudioManager.h"
#include "chuck_fft.h"
#include <math.h>

AudioSignalProcessor::AudioSignalProcessor()
{
    hanning(data.win_hanning,      256);
    hamming(data.win_hamming,      256);
    blackman(data.win_blackman,    256);
}

const Audio::AudioDataStruct& AudioSignalProcessor::Data()
{
    return data;
}

void AudioSignalProcessor::SetNormalization(Audio::AudioSettingsStruct* settings)
{
    for (int i = 0; i < 256; i++)
    {
        data.fft[i] = 0.0f;
        data.fft_nrml[i] = settings->nrml_ofst + (settings->nrml_scl * (i / 256.0f));
    }
}

void AudioSignalProcessor::Process(int FPS, Audio::AudioSettingsStruct* settings)
{
    float fft_tmp[512];

    for (int i = 0; i < 256; i++)
    {
        /*------------------*\
        | Clear the buffers  |
        \*------------------*/
        fft_tmp[i] = 0;

        /*----------------------*\
        | Decay previous values  |
        \*----------------------*/
        data.fft[i] = data.fft[i] * ((float(settings->decay) / 100.0f / (60 / FPS)));
    }

    AudioManager::get()->Capture(settings->audio_device, fft_tmp);

#ifdef _WIN32
    for (int i = 0; i < 512; i++)
    {
        fft_tmp[i] *= settings->amplitude;
    }
#else
    for (int i = 0; i < 512; i++)
    {
        fft_tmp[i] = (fft_tmp[i] - 128.0f) * (settings->amplitude / 128.0f);
    }
#endif

    /*----------------------*\
    | Apply selected window  |
    \*----------------------*/
    switch (settings->window_mode)
    {
    case 0:
        break;

    case 1:
        apply_window(fft_tmp, data.win_hanning, 256);
        break;

    case 2:
        apply_window(fft_tmp, data.win_hamming, 256);
        break;

    case 3:
        apply_window(fft_tmp, data.win_blackman, 256);
        break;

    default:
        break;
    }

    /*------------------------*\
    | Run the FFT calculation  |
    \*------------------------*/
    rfft(fft_tmp, 256, 1);

    fft_tmp[0] = fft_tmp[2];

    apply_window(fft_tmp, data.fft_nrml, 256);

    /*----------------------*\
    | Compute FFT magnitude  |
    \*----------------------*/
    for (int i = 0; i < 128; i += 2)
    {
        float fftmag;

        /*---------------------------------------------------------------------------------*\
        | Compute magnitude from real and imaginary components of FFT and apply simple LPF  |
        \*---------------------------------------------------------------------------------*/
        fftmag = (float)sqrt((fft_tmp[i] * fft_tmp[i]) + (fft_tmp[i + 1] * fft_tmp[i + 1]));

        /*----------------------------------------------------------------------------------------*\
        | Apply a slight logarithmic filter to minimize noise from very low amplitude frequencies  |
        \*----------------------------------------------------------------------------------------*/
        fftmag = ( 0.5f * log10(1.1f * fftmag) ) + ( 0.9f * fftmag );

        /*---------------------------*\
        | Limit FFT magnitude to 1.0  |
        \*---------------------------*/
        if (fftmag > 1.0f)
        {
            fftmag = 1.0f;
        }

        /*----------------------------------------------------------*\
        | Update to new values only if greater than previous values  |
        \*----------------------------------------------------------*/
        if (fftmag > data.fft[i*2])
        {
            data.fft[i*2] = fftmag;;
        }

        /*----------------------------*\
        | Prevent from going negative  |
        \*----------------------------*/
        if (data.fft[i*2] < 0.0f)
        {
            data.fft[i*2] = 0.0f;
        }

        /*--------------------------------------------------------------------*\
        | Set odd indexes to match their corresponding even index, as the FFT  |
        | input array uses two indices for one value (real+imaginary)          |
        \*--------------------------------------------------------------------*/
        data.fft[(i * 2) + 1] = data.fft[i * 2];
        data.fft[(i * 2) + 2] = data.fft[i * 2];
        data.fft[(i * 2) + 3] = data.fft[i * 2];
    }

    if (settings->avg_mode == 0)
    {
        /*--------------------------------------------*\
        | Apply averaging over given number of values  |
        \*--------------------------------------------*/
        unsigned int k;
        float sum1 = 0;
        float sum2 = 0;
        for (k = 0; k < settings->avg_size; k++)
        {
            sum1 += data.fft[k];
            sum2 += data.fft[255 - k];
        }
        /*------------------------------*\
        | Compute averages for end bars  |
        \*------------------------------*/
        sum1 = sum1 / k;
        sum2 = sum2 / k;
        for (k = 0; k < settings->avg_size; k++)
        {
            data.fft[k] = sum1;
            data.fft[255 - k] = sum2;
        }
        for (unsigned int i = 0; i < (256 - settings->avg_size); i += settings->avg_size)
        {
            float sum = 0;
            for (unsigned int j = 0; j < settings->avg_size; j += 1)
            {
                sum += data.fft[i + j];
            }

            float avg = sum / settings->avg_size;

            for (unsigned int j = 0; j < settings->avg_size; j += 1)
            {
                data.fft[i + j] = avg;
            }
        }
    }
    else if(settings->avg_mode == 1)
    {
        for (unsigned int i = 0; i < settings->avg_size; i++)
        {
            float sum1 = 0;
            float sum2 = 0;
            unsigned int j;

            for (j = 0; j <= i + settings->avg_size; j++)
            {
                sum1 += data.fft[j];
                sum2 += data.fft[255 - j];
            }
            data.fft[i] = sum1 / j;
            data.fft[255 - i] = sum2 / j;
        }
        for (unsigned int i = settings->avg_size; i < 256 - settings->avg_size; i++)
        {
            float sum = 0;
            for (unsigned int j = 1; j <= settings->avg_size; j++)
            {
                sum += data.fft[i - j];
                sum += data.fft[i + j];
            }
            sum += data.fft[i];

            data.fft[i] = sum / (2 * settings->avg_size + 1);
        }
    }

    for(int i = 0; i < 256; i++)
    {
        data.fft_fltr[i] = settings->equalizer[i/16] * (data.fft_fltr[i] + (settings->filter_constant * (data.fft[i] - data.fft_fltr[i])));
    }
}
