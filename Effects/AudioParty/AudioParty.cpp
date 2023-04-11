#include "AudioParty.h"
#include "ColorUtils.h"
#include "Audio/AudioManager.h"
#include "OpenRGBEffectSettings.h"

REGISTER_EFFECT(AudioParty);

AudioParty::AudioParty(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::AudioParty)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "AudioParty";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Movement, color change, effects according to audio zones.<br/>Blue zone: motion<br/>Green zone: colors<br/>Red zone: effects";
    EffectDetails.MaxSpeed     = 100;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.MaxSlider2Val = 50;
    EffectDetails.MinSlider2Val = 1;
    EffectDetails.Slider2Name   = "Color change speed";
    EffectDetails.HasCustomSettings = true;
    EffectDetails.SupportsRandom = false;

    SetSpeed(50);
    SetSlider2Val(25);

    /*--------------------------*\
    | Setup audio                |
    \*--------------------------*/

    memcpy(&audio_settings_struct, &OpenRGBEffectSettings::globalSettings.audio_settings,sizeof(Audio::AudioSettingsStruct));

    audio_signal_processor.SetNormalization(&audio_settings_struct);

    connect(&audio_settings, &AudioSettings::AudioDeviceChanged, this, &AudioParty::OnAudioDeviceChanged);
    connect(&audio_settings, &AudioSettings::NormalizationChanged,[=]{
        audio_signal_processor.SetNormalization(&audio_settings_struct);
    });

    audio_settings.SetSettings(&audio_settings_struct);

    color = ColorUtils::RandomRGBColor();

    connect(this, SIGNAL(UpdateGraphSignal(QPixmap)), this, SLOT(UpdateGraph(QPixmap)));

    ui->preview->setScaledContents(true);

    zones_slider = new ctkRangeSlider(Qt::Horizontal);
    zones_slider->setSingleStep(8);
    zones_slider->setMaximum(0);
    zones_slider->setMaximum(256);
    zones_slider->setValues(motion_zone_stop, color_zone_stop);

    ui->zones_layout->addWidget(zones_slider);

    connect(zones_slider,SIGNAL(valuesChanged(int,int)), this, SLOT(ZonesChanged(int, int)));

    /*------------------------------------*\
    | Init preview                         |
    \*------------------------------------*/
    QImage image(256, 64, QImage::Format_RGB30);
    image.fill(Qt::black);

    QPixmap pixmap;
    pixmap.convertFromImage(image);

    ui->preview->setPixmap(pixmap);
    ui->preview->setScaledContents(true);
}

AudioParty::~AudioParty()
{
    delete ui;
}

void AudioParty::EffectState(const bool state)
{
    EffectEnabled = state;
    state ? Start() : Stop();
}

void AudioParty::Start()
{
    if(audio_settings_struct.audio_device >= 0)
    {
        AudioManager::get()->RegisterClient(audio_settings_struct.audio_device, this);
    }
}

void AudioParty::Stop()
{
    if(audio_settings_struct.audio_device >= 0)
    {
        AudioManager::get()->UnRegisterClient(audio_settings_struct.audio_device, this);
    }
}

void AudioParty::OnAudioDeviceChanged(int value)
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

void AudioParty::UpdateGraph(QPixmap pixmap)
{
    ui->preview->setPixmap(pixmap);
}

void AudioParty::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    audio_signal_processor.Process(FPS, &audio_settings_struct);
    const Audio::AudioDataStruct& audio_data = audio_signal_processor.Data();

    /*--------------------------------------------*\
    | Generate preview                             |
    \*--------------------------------------------*/
    RGBColor color;
    QImage image(256, 64, QImage::Format_RGB30);
    QPixmap pixmap;

    for(unsigned int i = 0; i < 256; i++)
    {
        float value = audio_data.fft_fltr[i] * 100;

        color = i < motion_zone_stop ?
                    ToRGBColor(255,0,0) : i < color_zone_stop ?
                        ToRGBColor(0,255,0) : ToRGBColor(0, 0, 255);

        for(int y = 0; y < 64; y++)
        {
            if(y==round(effect_threshold*100)){
                image.setPixel(i, 63 - y, ToRGBColor(255,255,255));
            }
            else if(value > y || y == 0){
                image.setPixel(i , 63 - y, color);
            } else {
                image.setPixel(i, 63 - y, ColorUtils::OFF());
            }
        }

    }

    pixmap.convertFromImage(image);

    emit UpdateGraphSignal(pixmap);

    for(ControllerZone* controller_zone : controller_zones)
    {
        zone_type ZT = controller_zone->type();

        if(ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            unsigned int width = controller_zone->leds_count();
            unsigned int height = 1;

            for(unsigned int i = 0; i < width; i++)
            {
                RGBColor color = GetColor(i, 0, width, height);
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
                    RGBColor color = GetColor(w, h, width, height);

                    unsigned int led_num = map[h * width + w];
                    controller_zone->SetLED(led_num, color, Brightness, Temperature, Tint);
                }
            }
        }
    }

    float delta = (float) Speed / (float) FPS;
    float color_delta = (float) Slider2Val / (float) FPS;

    /*--------------------------------------------*\
    | X axis motion                                |
    \*--------------------------------------------*/

    int c = 0;

    for (unsigned int i = 0; i < motion_zone_stop ; i+= audio_settings_struct.avg_size)
    {
        if(audio_data.fft_fltr[i] > audio_data.fft_fltr[i + audio_settings_struct.avg_size]){
            int mult = c % 2 == 0 ? 1:-1;
            x_shift += audio_data.fft[i] * delta * mult;
            break;
        }
        c++;
    }

    /*--------------------------------------------*\
    | Color motion                                 |
    \*--------------------------------------------*/
    c = 0;
    for (unsigned int i = motion_zone_stop; i < color_zone_stop; i+= audio_settings_struct.avg_size)
    {
        if(audio_data.fft_fltr[i] > audio_data.fft_fltr[i + audio_settings_struct.avg_size]){
            int mult = c % 2 == 0 ? 1:-1;
            color_shift += audio_data.fft_fltr[i] * color_delta * mult;
            break;
        }
        c++;
    }

    /*--------------------------------------------*\
    | Trigger effects                              |
    \*--------------------------------------------*/
    for (unsigned int i = color_zone_stop; i < 256; i+= audio_settings_struct.avg_size)
    {
        if(audio_data.fft_fltr[i] > effect_threshold && audio_data.fft_fltr[i] > audio_data.fft_fltr[i + audio_settings_struct.avg_size] && effect_progress >= 1){
            effect_idx = rand()%7;
            effect_progress = 0;
            break;
        }
        c++;
    }

    /*--------------------------------------------*\
    | Run effects                                  |
    \*--------------------------------------------*/
    if(effect_progress < 1)
    {
        effect_progress += 0.1 * delta;
    }

}


RGBColor AudioParty::GetColor(float x, float y, float w, float h)
{
    float pi = 3.14159265359;
    float pos_x = x / w;
    float s = 0.5 * (1 + sin(pos_x * divisions * pi + x_shift));

    /*--------------------------------------------*\
    | Color motion                                 |
    \*--------------------------------------------*/
    hsv_t hsv;
    hsv.hue = 180 + sin(y/h + color_shift) * 180;
    hsv.saturation = 255;
    hsv.value = 255;

    RGBColor effect_color = ColorUtils::OFF();

    if(effect_progress < 1)
    {
        switch (effect_idx) {
        case 0: {
            // moving bar LTR

            if(fabs(x - effect_progress * w) <= 1){
                effect_color = ToRGBColor(255,255,255);
            }

            break;
        }
        case 1: {
            // moving bar RTL

            if(fabs(x - (w - effect_progress * w)) <= 1){
                effect_color = ToRGBColor(255,255,255);
            }

            break;
        }
        case 2: {
            // moving bar UTB


            if(fabs(y - effect_progress * h) <= 1){
                effect_color = ToRGBColor(255,255,255);
            }

            break;
        }
        case 3: {
            // moving bar BTU

            if(fabs(y - (h - effect_progress * h)) <= 1){
                effect_color = ToRGBColor(255,255,255);
            }

            break;
        }
        case 4: {
            // random rgb
            effect_color = ColorUtils::RandomRGBColor();
            break;
        }
        case 5: {
            // random white
            float value = ((double) rand() / (RAND_MAX));
            effect_color = ColorUtils::Enlight(ToRGBColor(255,255,255), value);

            break;
        }
        case 6: {
            // blink
            float value = 1 - effect_progress;
            effect_color = ColorUtils::Enlight(ToRGBColor(255,255,255), value);

            break;
        }


        default: break;

        }
    }

    RGBColor c = ColorUtils::Screen(hsv2rgb(&hsv),effect_color);

    return ColorUtils::Interpolate(c, ColorUtils::OFF(), s);
}

void AudioParty::on_audio_settings_clicked()
{
    audio_settings.show();
}

void AudioParty::on_divisions_valueChanged(int value)
{
    divisions = value;
}

void AudioParty::on_effect_threshold_valueChanged(double value)
{
    effect_threshold = value;
}

void AudioParty::ZonesChanged(int min, int max)
{
    motion_zone_stop = min;
    color_zone_stop = max;
}

json AudioParty::SaveCustomSettings()
{
    json settings;

    settings["divisions"] = divisions;
    settings["effect_threshold"] = effect_threshold;
    settings["motion_zone_stop"] = motion_zone_stop;
    settings["color_zone_stop"] = color_zone_stop;
    settings["audio_settings"] = audio_settings_struct;

    return settings;
}

void AudioParty::LoadCustomSettings(json Settings)
{
    if (Settings.contains("divisions"))
        ui->divisions->setValue(Settings["divisions"]);

    if (Settings.contains("effect_threshold"))
        ui->effect_threshold->setValue(Settings["effect_threshold"]);

    if (Settings.contains("motion_zone_stop"))
        zones_slider->setMinimumValue(Settings["motion_zone_stop"]);

    if (Settings.contains("color_zone_stop"))
        zones_slider->setMaximumValue(Settings["color_zone_stop"]);

    if (Settings.contains("audio_settings"))
    {
        audio_settings_struct = Settings["audio_settings"];
        audio_settings.SetSettings(&audio_settings_struct);
    }
}
