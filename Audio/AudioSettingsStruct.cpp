#include "AudioSettingsStruct.h"

namespace Audio {
    void to_json(json& j, const AudioSettingsStruct& s)
    {
        j = json{
            {"audio_device"     ,   s.audio_device      },
            {"amplitude"        ,   s.amplitude         },
            {"avg_mode"         ,   s.avg_mode          },
            {"avg_size"         ,   s.avg_size          },
            {"window_mode"      ,   s.window_mode       },
            {"decay"            ,   s.decay             },
            {"filter_constant"  ,   s.filter_constant   },
            {"nrml_ofst"        ,   s.nrml_ofst         },
            {"nrml_scl"         ,   s.nrml_scl          },
            {"equalizer"        ,   s.equalizer         }
        };
    }

    void from_json(const json& j, AudioSettingsStruct& s)
    {
        j.at("audio_device")        .get_to(s.audio_device);
        j.at("amplitude")           .get_to(s.amplitude);
        j.at("avg_mode")            .get_to(s.avg_mode);
        j.at("avg_size")            .get_to(s.avg_size);
        j.at("window_mode")         .get_to(s.window_mode);
        j.at("decay")               .get_to(s.decay);
        j.at("filter_constant")     .get_to(s.filter_constant);
        j.at("nrml_ofst")           .get_to(s.nrml_ofst);
        j.at("nrml_scl")            .get_to(s.nrml_scl);
        j.at("equalizer")           .get_to(s.equalizer);
    }
}
