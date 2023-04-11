#include "AudioStar.h"
#include "ColorUtils.h"
#include "Audio/AudioManager.h"
#include "OpenRGBEffectSettings.h"

REGISTER_EFFECT(AudioStar);

AudioStar::AudioStar(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::AudioStar)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "AudioStar";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Star audio visualizer (frequency based) with an edge beat";
    EffectDetails.MaxSpeed     = 100;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.HasCustomSettings = true;
    EffectDetails.SupportsRandom = false;

    ui->edge_beatFrame->setVisible(false);

    SetSpeed(50);

    /*--------------------------*\
    | Setup audio                |
    \*--------------------------*/

    memcpy(&audio_settings_struct, &OpenRGBEffectSettings::globalSettings.audio_settings,sizeof(Audio::AudioSettingsStruct));

    audio_signal_processor.SetNormalization(&audio_settings_struct);

    connect(&audio_settings, &AudioSettings::AudioDeviceChanged, this, &AudioStar::OnAudioDeviceChanged);
    connect(&audio_settings, &AudioSettings::NormalizationChanged,[=]{
        audio_signal_processor.SetNormalization(&audio_settings_struct);
    });

    audio_settings.SetSettings(&audio_settings_struct);
}

AudioStar::~AudioStar()
{
    delete ui;
}

void AudioStar::EffectState(const bool state)
{
    EffectEnabled = state;
    state ? Start() : Stop();
}

void AudioStar::Start()
{
    if(audio_settings_struct.audio_device >= 0)
    {
        AudioManager::get()->RegisterClient(audio_settings_struct.audio_device, this);
    }
}

void AudioStar::Stop()
{
    if(audio_settings_struct.audio_device >= 0)
    {
        AudioManager::get()->UnRegisterClient(audio_settings_struct.audio_device, this);
    }
}

void AudioStar::OnAudioDeviceChanged(int value)
{
    bool was_running = EffectEnabled;

    if(EffectEnabled)
    {
        Stop();
    }

    audio_settings_struct.audio_device = value;

    if(was_running)
    {
        Start();
    }
}

void AudioStar::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    audio_signal_processor.Process(FPS, &audio_settings_struct);

    amp = 0;

    for(int i = 0; i < 256; i += audio_settings_struct.avg_size)
    {
        amp += audio_signal_processor.Data().fft_fltr[i];
    }

    for(ControllerZone* controller_zone : controller_zones)
    {
        zone_type ZT = controller_zone->type();

        if(ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            unsigned int width = controller_zone->leds_count();
            unsigned int height = 1;

            for(unsigned int i = 0; i < width; i++)
            {
                RGBColor color = GetColor(i, 0.5, width, height);
                controller_zone->SetLED(i, color, Brightness, Temperature, Tint);
            }

        }
        else if(ZT == ZONE_TYPE_MATRIX)
        {
            unsigned int width = controller_zone->matrix_map_width();
            unsigned int height = controller_zone->matrix_map_height();
            unsigned int * map = controller_zone->map();

            for(unsigned int w = 0; w <  width; w++)
            {
                for(unsigned int h = 0; h < height; h++)
                {
                    RGBColor color = GetColor(w, h, width-1, height-1);

                    unsigned int led_num = map[h * width + w];
                    controller_zone->SetLED(led_num, color, Brightness, Temperature, Tint);
                }
            }
        }
    }

    time += (float) Speed / (float) FPS;
}

RGBColor AudioStar::GetColor(float x, float y, float w, float h)
{
    hsv_t hsv;

    float cx = w * 0.5;
    float cy = h * 0.5;

    float pi = 3.14;

    float angle = fabs(atan2((x - cx), (y  - cy)));

    double freq_amp = audio_signal_processor.Data().fft_fltr[(int)(256 * (angle / (pi*2)))];

    hsv.hue = (int)((angle/pi)*360 + time) % 360;
    hsv.saturation = 255;
    hsv.value = std::min<float>(255, pow(freq_amp, 1/(amp+1)) * 255);

    RGBColor color = RGBColor(hsv2rgb(&hsv));

    if(edge_beat)
    {
        bool is_edge = x == 0 || x == w || y == 0 || y == h ;

        if (is_edge)
        {
            hsv_t edge;

            edge.hue = edge_beat_hue;
            edge.saturation = edge_beat_saturation;
            edge.value = 255 * std::min<float>(
                        1.,
                        0.01 * edge_beat_sensivity * (audio_signal_processor.Data().fft_fltr[0] + audio_signal_processor.Data().fft_fltr[8]));

            return ColorUtils::Screen(color, RGBColor(hsv2rgb(&edge)));
        }
    }

    return color;
}

void AudioStar::on_edge_beat_hue_valueChanged(int value)
{
    edge_beat_hue = value;
}

void AudioStar::on_edge_beat_saturation_valueChanged(int value)
{
    edge_beat_saturation = value;
}

void AudioStar::on_edge_beat_sensivity_valueChanged(int value)
{
    edge_beat_sensivity = value;
}

void AudioStar::on_edge_beat_stateChanged(int value)
{
    edge_beat = value;

    ui->edge_beatFrame->setVisible(value);
}

void AudioStar::on_audio_settings_clicked()
{
    audio_settings.show();
}

json AudioStar::SaveCustomSettings()
{
    json settings;

    settings["edge_beat"] = edge_beat;
    settings["edge_beat_sensivity"] = edge_beat_sensivity;
    settings["edge_beat_saturation"] = edge_beat_saturation;
    settings["edge_beat_hue"] = edge_beat_hue;
    settings["audio_settings"] = audio_settings_struct;

    return settings;
}

void AudioStar::LoadCustomSettings(json Settings)
{
    if (Settings.contains("edge_beat"))
        ui->edge_beat->setChecked(Settings["edge_beat"]);

    if (Settings.contains("edge_beat_sensivity"))
        ui->edge_beat_sensivity->setValue(Settings["edge_beat_sensivity"]);

    if (Settings.contains("edge_beat_saturation"))
        ui->edge_beat_saturation->setValue(Settings["edge_beat_saturation"]);

    if (Settings.contains("edge_beat_hue"))
        ui->edge_beat_hue->setValue(Settings["edge_beat_hue"]);

    if (Settings.contains("audio_settings"))
    {
        audio_settings_struct = Settings["audio_settings"];
        audio_settings.SetSettings(&audio_settings_struct);
    }
}
