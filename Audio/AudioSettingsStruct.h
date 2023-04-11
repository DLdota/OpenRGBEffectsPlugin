#ifndef AUDIOSETTINGSSTRUCT_H
#define AUDIOSETTINGSSTRUCT_H

#include "json.hpp"

using json = nlohmann::json;

namespace Audio
{
    enum
    {
        AVERAGE_MODE_BINNING        = 0,
        AVERAGE_MODE_LOW_PASS       = 1
    };

    enum
    {
        FFT_WINDOW_MODE_NONE        = 0,
        FFT_WINDOW_MODE_HANNING     = 1,
        FFT_WINDOW_MODE_HAMMING     = 2,
        FFT_WINDOW_MODE_BLACKMAN    = 3
    };

    struct AudioSettingsStruct
    {
        int           audio_device     = -1;

        unsigned int  amplitude        = 100;
        unsigned int  avg_mode         = AVERAGE_MODE_BINNING;
        unsigned int  avg_size         = 8;
        unsigned int  window_mode      = FFT_WINDOW_MODE_NONE;
        unsigned int  decay            = 80;

        float         filter_constant  = 1.0f;
        float         nrml_ofst        = 0.04f;
        float         nrml_scl         = 0.5f;

        float         equalizer[16]    =
        {
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f
        };
    };

    void to_json(json& j, const AudioSettingsStruct& s);
    void from_json(const json& j, AudioSettingsStruct& s);
}

#endif // AUDIOSETTINGSSTRUCT_H
