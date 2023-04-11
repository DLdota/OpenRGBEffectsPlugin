#include "SwirlCirclesAudio.h"
#include "ColorUtils.h"
#include "Audio/AudioManager.h"
#include "OpenRGBEffectSettings.h"

REGISTER_EFFECT(SwirlCirclesAudio);

SwirlCirclesAudio::SwirlCirclesAudio(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::SwirlCirclesAudio)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "SwirlCirclesAudio";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Rotating circles reacting to audio";
    EffectDetails.IsReversable = true;
    EffectDetails.MaxSpeed     = 100;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.UserColors   = 2;
    EffectDetails.MaxSlider2Val = 100;
    EffectDetails.MinSlider2Val = 1;
    EffectDetails.Slider2Name   = "Glow";
    EffectDetails.HasCustomSettings = true;

    SetSpeed(50);
    SetSlider2Val(50);

    /*--------------------------*\
    | Setup audio                |
    \*--------------------------*/

    memcpy(&audio_settings_struct, &OpenRGBEffectSettings::globalSettings.audio_settings,sizeof(Audio::AudioSettingsStruct));

    audio_signal_processor.SetNormalization(&audio_settings_struct);

    connect(&audio_settings, &AudioSettings::AudioDeviceChanged, this, &SwirlCirclesAudio::OnAudioDeviceChanged);
    connect(&audio_settings, &AudioSettings::NormalizationChanged,[=]{
        audio_signal_processor.SetNormalization(&audio_settings_struct);
    });

    audio_settings.SetSettings(&audio_settings_struct);
}

SwirlCirclesAudio::~SwirlCirclesAudio()
{
    delete ui;
}

void SwirlCirclesAudio::EffectState(const bool state)
{
    EffectEnabled = state;
    state ? Start() : Stop();
}

void SwirlCirclesAudio::Start()
{
    if(audio_settings_struct.audio_device >= 0)
    {
        AudioManager::get()->RegisterClient(audio_settings_struct.audio_device, this);
    }
}

void SwirlCirclesAudio::Stop()
{
    if(audio_settings_struct.audio_device >= 0)
    {
        AudioManager::get()->UnRegisterClient(audio_settings_struct.audio_device, this);
    }
}

void SwirlCirclesAudio::OnAudioDeviceChanged(int value)
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

void SwirlCirclesAudio::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    audio_signal_processor.Process(FPS, &audio_settings_struct);

    current_level = 0.f;

    for(int i = 0; i < 256; i++)
    {
        current_level += audio_signal_processor.Data().fft_fltr[i];
    }

    for(ControllerZone* controller_zone : controller_zones)
    {
        bool reverse = controller_zone->reverse;
        zone_type ZT = controller_zone->type();

        if(ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            unsigned int width = controller_zone->leds_count();
            unsigned int height = 1;

            float hx = 0.5 * width;
            float hy = 0.5 * height;

            float y1 = hy + hy * sin(reverse ? -progress : progress);
            float x1 = hx + hx * cos(reverse ? -progress : progress);

            for(unsigned int i = 0; i < width; i++)
            {
                RGBColor color = GetColor(i, 0, width, height, x1, y1);
                controller_zone->SetLED(i, color, Brightness);
            }

        }
        else if(ZT == ZONE_TYPE_MATRIX)
        {
            unsigned int width = controller_zone->matrix_map_width();
            unsigned int height = controller_zone->matrix_map_height();

            float hx = 0.5 * width;
            float hy = 0.5 * height;

            float y1 = hy + hy * sin(reverse ? -progress : progress);
            float x1 = hx + hx * cos(reverse ? -progress : progress);

            for(unsigned int h = 0; h < height; h++)
            {
                for(unsigned int w = 0; w <  width; w++)
                {
                    RGBColor color = GetColor(w, h, width, height, x1, y1);

                    unsigned int led_num = controller_zone->map()[h * width + w];
                    controller_zone->SetLED(led_num,color, Brightness);
                }
            }

        }
    }

    progress +=  0.1 * (float) Speed / (float) FPS;

    if(RandomColorsEnabled)
    {
        hsv1.hue++;
        hsv2.hue++;
    }
}

RGBColor SwirlCirclesAudio::GetColor(unsigned int x, unsigned int y, unsigned int w, unsigned int h, float x1, float y1)
{
    float glow_mult = 0.001 * Slider2Val;

    double distance1 = sqrt(pow(x1 - x, 2) + pow(y1 - y, 2));
    float distance1_percent = distance1 < radius ? 1 / (0.000001 + current_level) : pow(distance1 / (h+w), glow_mult * current_level);

    hsv_t hsv_tmp;

    hsv_tmp.value = std::min<float>(255 , std::max<float>(0, hsv1.value * (1 - distance1_percent)));
    hsv_tmp.hue = hsv1.hue;
    hsv_tmp.saturation = hsv1.saturation;

    RGBColor color1 = RGBColor(hsv2rgb(&hsv_tmp));

    float y2 = h - y1;
    float x2 = w - x1;

    double distance2 = sqrt(pow(x2 - x, 2) + pow(y2 - y, 2));
    float distance2_percent = distance2 < radius ? 1 / (0.000001 + current_level) : pow(distance2 / (h+w), glow_mult * current_level);

    hsv_tmp.value = std::min<float>(255 , std::max<float>(0, hsv2.value * (1 - distance2_percent)));
    hsv_tmp.hue = hsv2.hue;
    hsv_tmp.saturation = hsv2.saturation;

    RGBColor color2 = RGBColor(hsv2rgb(&hsv_tmp));

    return ColorUtils::Screen(color1, color2);
}

void SwirlCirclesAudio::SetUserColors(std::vector<RGBColor> colors)
{
    UserColors = colors;

    ResetUserColors();
}

void SwirlCirclesAudio::SetRandomColorsEnabled(bool value)
{
    RandomColorsEnabled = value;

    if(!RandomColorsEnabled)
    {
        ResetUserColors();
    }
    else
    {
        hsv1.hue = 0;
        hsv1.value = 255;
        hsv1.saturation = 255;

        hsv2.hue = 180;
        hsv2.value = 255;
        hsv2.saturation = 255;
    }
}

void SwirlCirclesAudio::ResetUserColors()
{
    rgb2hsv(UserColors[0], &hsv1);
    rgb2hsv(UserColors[1], &hsv2);
}

void SwirlCirclesAudio::on_radius_valueChanged(int value)
{
    radius = value;
}

void SwirlCirclesAudio::on_audio_settings_clicked()
{
    audio_settings.show();
}

json SwirlCirclesAudio::SaveCustomSettings()
{
    json settings;

    settings["radius"]              = radius;
    settings["audio_settings"] = audio_settings_struct;

    return settings;
}

void SwirlCirclesAudio::LoadCustomSettings(json settings)
{
    if (settings.contains("radius"))            ui->radius->setValue(settings["radius"]);

    if (settings.contains("audio_settings"))
    {
        audio_settings_struct = settings["audio_settings"];
        audio_settings.SetSettings(&audio_settings_struct);
    }
}
