#include "AudioVisualizer.h"
#include "Colors.h"
#include "Audio/AudioManager.h"
#include "OpenRGBEffectSettings.h"

/*---------------------------------------------------------*\
|  Processing Code for Keyboard Visualizer                  |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 12/11/2016       |
|  Modded by CoffeeIsLife (gitlab.com/CoffeeIsLife)         |
\*---------------------------------------------------------*/

REGISTER_EFFECT(AudioVisualizer);

AudioVisualizer::AudioVisualizer(QWidget* parent):
    RGBEffect(parent),
    ui(new Ui::AudioVisualizerUi)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "Audio Visualizer";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Display audio equalizer on your devices. A ported version of <a href=\"https://gitlab.com/CalcProgrammer1/KeyboardVisualizer\">KeyboardVisualizer</a>";
    EffectDetails.HasCustomSettings = true;
    EffectDetails.SupportsRandom = false;

    pixels_render = &pixels_vs1;
    pixels_out = &pixels_vs2;

    background_timer     = 0;
    bkgd_step            = 0;

    /*---------------------*\
    | Setup default values  |
    \*---------------------*/
    ui->doubleSpinBox_Animation_Speed->setValue(100.0f);
    ui->spinBox_Background_Brightness->setValue(100);
    ui->doubleSpinBox_Background_Timeout->setValue(120);
    ui->checkBox_Reactive_Background->setChecked(false);
    ui->checkBox_Silent_Background->setChecked(false);

    for(int i = 0; i < VISUALIZER_NUM_PATTERNS; i++)
    {
        ui->comboBox_Background_Mode->addItem(visualizer_pattern_labels[i]);
        ui->comboBox_Foreground_Mode->addItem(visualizer_pattern_labels[i]);
    }

    ui->comboBox_Background_Mode->setCurrentIndex(VISUALIZER_PATTERN_ANIM_RAINBOW_SINUSOIDAL);
    ui->comboBox_Foreground_Mode->setCurrentIndex(VISUALIZER_PATTERN_STATIC_GREEN_YELLOW_RED);

    for(int i = 0; i < VISUALIZER_NUM_SINGLE_COLOR; i++)
    {
        ui->comboBox_Single_Color_Mode->addItem(visualizer_single_color_labels[i]);
    }

    ui->comboBox_Single_Color_Mode->setCurrentIndex(VISUALIZER_SINGLE_COLOR_FOLLOW_FOREGROUND);

    /*--------------------------*\
    | Setup preview              |
    \*--------------------------*/
    ui->preview->setScaledContents(true);
    connect(this, SIGNAL(UpdateGraphSignal()), this, SLOT(Update()));
    image = new QImage(256, 64, QImage::Format_RGB32);

    /*--------------------------*\
    | Setup audio                |
    \*--------------------------*/
    memcpy(&audio_settings_struct, &OpenRGBEffectSettings::globalSettings.audio_settings,sizeof(Audio::AudioSettingsStruct));

    audio_signal_processor.SetNormalization(&audio_settings_struct);

    connect(&audio_settings, &AudioSettings::AudioDeviceChanged, this, &AudioVisualizer::OnAudioDeviceChanged);
    connect(&audio_settings, &AudioSettings::NormalizationChanged,[=]{
        audio_signal_processor.SetNormalization(&audio_settings_struct);
    });

    audio_settings.SetSettings(&audio_settings_struct);
}

AudioVisualizer::~AudioVisualizer()
{
    delete ui;
}

void AudioVisualizer::EffectState(const bool state)
{
    ZoneMaps.clear();
    EffectEnabled = state;
    state ? Start() : Stop();
}

void AudioVisualizer::Start()
{
    if(audio_settings_struct.audio_device >= 0)
    {
        AudioManager::get()->RegisterClient(audio_settings_struct.audio_device, this);
    }
}

void AudioVisualizer::Stop()
{
    if(audio_settings_struct.audio_device >= 0)
    {
        AudioManager::get()->UnRegisterClient(audio_settings_struct.audio_device, this);
    }
}

void AudioVisualizer::OnAudioDeviceChanged(int value)
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

void AudioVisualizer::StepEffect(std::vector<ControllerZone*> controller_zones)
{

    audio_signal_processor.Process(FPS, &audio_settings_struct);

    if (ZoneMaps.size() != controller_zones.size())
    {
        ZoneMaps.clear();

        for(unsigned int i = 0; i < controller_zones.size(); i++)
        {
            ZoneMaps.push_back(nullptr);
        }
    }

    /*-------------------------*\
    | Overflow background step  |
    \*-------------------------*/
    if (bkgd_step >= 360.0f) bkgd_step = 0.0f;
    if (bkgd_step < 0.0f) bkgd_step = 360.0f;

    /*-----------------------*\
    | Draw active background  |
    \*-----------------------*/
    DrawPattern(bkgd_mode, bkgd_bright, &pixels_bg);

    /*------------------------*\
    | Draw active foreground   |
    \*------------------------*/
    DrawPattern(frgd_mode, 100, &pixels_fg);

    float brightness = audio_signal_processor.Data().fft_fltr[5];

    /*----------------------------------------------------------------------*\
    | If music isn't playing, fade in the single color LEDs after 2 seconds  |
    \*----------------------------------------------------------------------*/
    background_timer += 60.f / FPS;

    if (shutdown_flag == true)
    {
        if (background_timer >= background_timeout)
        {
            brightness = 0.0f;
        }
        else
        {
            brightness = ((background_timeout - background_timer) / (1.0f * background_timeout));
        }
    }
    else if (background_timeout > 0)
    {
        for (int i = 0; i < 128; i++)
        {
            if (audio_signal_processor.Data().fft_fltr[2 * i] >= 0.0001f)
            {
                background_timer = 0;
            }
        }
        if (background_timer >= background_timeout)
        {
            if (background_timer >= (3 * background_timeout))
            {
                background_timer = (3 * background_timeout);
            }
            brightness = (background_timer - background_timeout) / (2.0f * background_timeout);
        }
    }

    /*------------------------------------------------------*\
    | Loop through all 256x64 pixels in visualization image  |
    \*------------------------------------------------------*/
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            /*-----------------------------*\
            | Draw Spectrograph Foreground  |
            \*-----------------------------*/
            if (audio_signal_processor.Data().fft_fltr[x] >((1 / 64.0f)*(64.0f - y)))
            {
                if (shutdown_flag == true)
                {
                    int in_color = pixels_fg.pixels[y][x];
                    pixels_render->pixels[y][x] = ToRGBColor((int(brightness * RGBGetBValue(in_color))), (int(brightness * RGBGetGValue(in_color))), (int(brightness * RGBGetRValue(in_color))));
                }
                else
                {
                    pixels_render->pixels[y][x] = pixels_fg.pixels[y][x];
                }
            }
            else
            {
                if(reactive_bkgd || silent_bkgd)
                {
                    if (!silent_bkgd || ((background_timer >= background_timeout) && (background_timeout > 0)))
                    {
                        int in_color = pixels_bg.pixels[y][x];
                        pixels_render->pixels[y][x] = ToRGBColor((int(brightness * RGBGetRValue(in_color))), (int(brightness * RGBGetGValue(in_color))), (int(brightness * RGBGetBValue(in_color))));
                    }
                    else
                    {
                        pixels_render->pixels[y][x] = ToRGBColor(0, 0, 0);
                    }
                }
                else
                {
                    pixels_render->pixels[y][x] = pixels_bg.pixels[y][x];
                }

            }

            /*--------------------------*\
            | Draw Bar Graph Foreground  |
            \*--------------------------*/
            if (y == ROW_IDX_BAR_GRAPH)
            {
                if (x < 128)
                {
                    if ((audio_signal_processor.Data().fft_fltr[5] - 0.05f) >((1 / 128.0f)*(127-x)))
                    {
                        if (shutdown_flag == true)
                        {
                            int in_color = pixels_fg.pixels[y][x];
                            pixels_render->pixels[y][x] = ToRGBColor((int(brightness * RGBGetBValue(in_color))), (int(brightness * RGBGetGValue(in_color))), (int(brightness * RGBGetRValue(in_color))));
                        }
                        else
                        {
                            pixels_render->pixels[y][x] = pixels_fg.pixels[y][x];
                        }
                    }
                    else
                    {
                        if (reactive_bkgd || silent_bkgd)
                        {
                            if (!silent_bkgd || ((background_timer >= background_timeout) && (background_timeout > 0)))
                            {
                                int in_color = pixels_bg.pixels[y][x];
                                pixels_render->pixels[y][x] = ToRGBColor((int(brightness * RGBGetBValue(in_color))), (int(brightness * RGBGetGValue(in_color))), (int(brightness * RGBGetRValue(in_color))));
                            }
                            else
                            {
                                pixels_render->pixels[y][x] = ToRGBColor(0, 0, 0);
                            }
                        }
                        else
                        {
                            pixels_render->pixels[y][x] = pixels_bg.pixels[y][x];
                        }
                    }
                }
                else
                {
                    if ((audio_signal_processor.Data().fft_fltr[5] - 0.05f) >((1 / 128.0f)*((x-128))))
                    {
                        if (shutdown_flag == true)
                        {
                            int in_color = pixels_fg.pixels[y][x];
                            pixels_render->pixels[y][x] = ToRGBColor((int(brightness * RGBGetBValue(in_color))), (int(brightness * RGBGetGValue(in_color))), (int(brightness * RGBGetRValue(in_color))));
                        }
                        else
                        {
                            pixels_render->pixels[y][x] = pixels_fg.pixels[y][x];
                        }
                    }
                    else
                    {
                        if (reactive_bkgd || silent_bkgd)
                        {
                            if (!silent_bkgd || (background_timer >= background_timeout))
                            {
                                int in_color = pixels_bg.pixels[y][x];
                                pixels_render->pixels[y][x] = ToRGBColor((int(brightness * RGBGetBValue(in_color))), (int(brightness * RGBGetGValue(in_color))), (int(brightness * RGBGetRValue(in_color))));
                            }
                            else
                            {
                                pixels_render->pixels[y][x] = ToRGBColor(0, 0, 0);
                            }
                        }
                        else
                        {
                            pixels_render->pixels[y][x] = pixels_bg.pixels[y][x];
                        }
                    }
                }
            }
        }
    }

    if (single_color_mode == VISUALIZER_SINGLE_COLOR_FOLLOW_BACKGROUND)
    {
        brightness = (bkgd_bright / 100.0f) * brightness;
    }

    if ((background_timeout <= 0 ) || (background_timer < background_timeout))
    {
        /*--------------------------------------------------------*\
        | Draw brightness based visualizer for single LED devices  |
        \*--------------------------------------------------------*/
        switch (single_color_mode)
        {
            case VISUALIZER_SINGLE_COLOR_BLACK:
                DrawSingleColorStatic(brightness, COLOR_BLACK, pixels_render);
                break;

            case VISUALIZER_SINGLE_COLOR_WHITE:
                DrawSingleColorStatic(brightness, COLOR_WHITE, pixels_render);
                break;

            case VISUALIZER_SINGLE_COLOR_RED:
                DrawSingleColorStatic(brightness, COLOR_RED, pixels_render);
                break;

            case VISUALIZER_SINGLE_COLOR_ORANGE:
                DrawSingleColorStatic(brightness, COLOR_ORANGE, pixels_render);
                break;

            case VISUALIZER_SINGLE_COLOR_YELLOW:
                DrawSingleColorStatic(brightness, COLOR_YELLOW, pixels_render);
                break;

            case VISUALIZER_SINGLE_COLOR_GREEN:
                DrawSingleColorStatic(brightness, COLOR_LIME, pixels_render);
                break;

            case VISUALIZER_SINGLE_COLOR_CYAN:
                DrawSingleColorStatic(brightness, COLOR_CYAN, pixels_render);
                break;

            case VISUALIZER_SINGLE_COLOR_BLUE:
                DrawSingleColorStatic(brightness, COLOR_BLUE, pixels_render);
                break;

            case VISUALIZER_SINGLE_COLOR_PURPLE:
                DrawSingleColorStatic(brightness, COLOR_PURPLE, pixels_render);
                break;

            case VISUALIZER_SINGLE_COLOR_ELECTRIC_AQUAMARINE:
                DrawSingleColorStatic(brightness, COLOR_ELECTRIC_ULTRAMARINE, pixels_render);
                break;

            case VISUALIZER_SINGLE_COLOR_BACKGROUND:
                /*----------------------------------------------------------*\
                | Intentionally do nothing, leave the background unmodified  |
                \*----------------------------------------------------------*/
                break;

            case VISUALIZER_SINGLE_COLOR_FOLLOW_BACKGROUND:
                DrawSingleColorBackground(brightness, &pixels_bg, pixels_render);
                break;

            case VISUALIZER_SINGLE_COLOR_FOLLOW_FOREGROUND:
                DrawSingleColorForeground(brightness, &pixels_fg, pixels_render);
                break;

            default: break;
        }
    }

    /*-------------*\
    | Swap buffers  |
    \*-------------*/
    if (pixels_render == &pixels_vs1)
    {
        pixels_render = &pixels_vs2;
        pixels_out = &pixels_vs1;
    }
    else
    {
        pixels_render = &pixels_vs1;
        pixels_out = &pixels_vs2;
    }

    /*--------------------------*\
    | Increment background step  |
    \*--------------------------*/
    bkgd_step = bkgd_step + ((anim_speed * 100) / (100.0f * FPS));

    int i = 0;

    for(ControllerZone* controller_zone: controller_zones)
    {
        int                 x_count                 = controller_zone->leds_count();
        int                 y_count                 = 0;
        zone_type           type                    = controller_zone->type();
        ZoneIndexType *     zone_index_map          = NULL;

        /*--------------------------------------------------------------*\
        | If matrix type and matrix mapping is valid, get X and Y count  |
        \*--------------------------------------------------------------*/
        if(type == ZONE_TYPE_MATRIX)
        {
            x_count     = controller_zone->matrix_map_width();
            y_count     = controller_zone->matrix_map_height();
        }

        if (ZoneMaps[i] == nullptr || x_count != ZoneMaps[i]->x_count || y_count != ZoneMaps[i]->y_count)
        {
            ZoneIndexType *   new_index_map             = new ZoneIndexType();
            new_index_map->x_count                      = x_count;
            new_index_map->y_count                      = y_count;

            if(type == ZONE_TYPE_MATRIX)
            {
                new_index_map->x_index                  = new int[x_count];
                new_index_map->y_index                  = new int[y_count];

                SetupMatrixGrid(x_count, y_count, new_index_map->x_index, new_index_map->y_index);
            }
            else if(type == ZONE_TYPE_LINEAR)
            {
                new_index_map->x_index                  = new int[x_count];

                SetupLinearGrid(x_count, new_index_map->x_index);
            }

            ZoneMaps[i] = new_index_map;
        }

        zone_index_map = ZoneMaps[i];

        switch (controller_zone->type())
        {
            case ZONE_TYPE_MATRIX:
                for (int y = 0; y < y_count; y++)
                {
                    for (int x = 0; x < x_count; x++)
                    {
                        unsigned int map_idx = (y * x_count) + x;
                        unsigned int color_idx = controller_zone->map()[map_idx];

                        if( color_idx != 0xFFFFFFFF )
                        {
                            controller_zone->SetLED(color_idx, pixels_out->pixels[zone_index_map->y_index[y]][zone_index_map->x_index[x]], Brightness, Temperature, Tint);
                        }
                    }
                }
                break;

            case ZONE_TYPE_SINGLE:
                for (int r = 0; r < x_count; r++)
                {
                    controller_zone->SetLED(r, pixels_out->pixels[ROW_IDX_SINGLE_COLOR][0], Brightness, Temperature, Tint);
                }
                break;

            case ZONE_TYPE_LINEAR:
                for (int x = 0; x < x_count; x++)
                {
                    controller_zone->SetLED(x, pixels_out->pixels[ROW_IDX_BAR_GRAPH][zone_index_map->x_index[x]], Brightness, Temperature, Tint);
                }
                break;

            default: break;
        }

        i++;
    }

    emit UpdateGraphSignal();
}

/*----------*\
| GUI slots  |
\*----------*/
void AudioVisualizer::Update()
{
    for(int x = 0; x < 256; x++)
    {
        for(int y = 0; y < 64; y++)
        {
            image->setPixelColor(x, y, ColorUtils::toQColor(pixels_out->pixels[y][x]));
        }
    }

    pixmap.convertFromImage(*image);
    ui->preview->setPixmap(pixmap);
}

void AudioVisualizer::on_spinBox_Background_Brightness_valueChanged(int value)
{
    bkgd_bright = value;
}

void AudioVisualizer::on_doubleSpinBox_Animation_Speed_valueChanged(double value)
{
    anim_speed = value;
}


void AudioVisualizer::DrawSolidColor(int bright, RGBColor color, vis_pixels *pixels)
{
    bright = (int)(bright * (255.0f / 100.0f));
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            pixels->pixels[y][x] = ToRGBColor((int(bright * RGBGetBValue(color)) / 256), (int(bright * RGBGetGValue(color)) / 256), (int(bright * RGBGetRValue(color)) / 256));
        }
    }
}

void AudioVisualizer::DrawSingleColorStatic(float amplitude, RGBColor in_color, vis_pixels *out_pixels)
{
    if (amplitude >= 1.0f)
    {
        amplitude = 1.0f;
    }
    else if (amplitude <= 0.0f)
    {
        amplitude = 0.0f;
    }

    int out_color = ToRGBColor((int(amplitude * RGBGetBValue(in_color))), (int(amplitude * RGBGetGValue(in_color))), (int(amplitude * RGBGetRValue(in_color))));
    for (int x = 0; x < 256; x++)
    {
        out_pixels->pixels[ROW_IDX_SINGLE_COLOR][x] = out_color;
    }
}

void AudioVisualizer::DrawSingleColorForeground(float amplitude, vis_pixels *fg_pixels, vis_pixels *out_pixels)
{
    if (amplitude >= 1.0f)
    {
        amplitude = 1.0f;
    }
    else if (amplitude <= 0.0f)
    {
        amplitude = 0.0f;
    }

    int idx = (int)(64.0f - (amplitude * 62.0f));
    int in_color = fg_pixels->pixels[idx][0];
    int out_color = ToRGBColor((int(amplitude * RGBGetBValue(in_color))), (int(amplitude * RGBGetGValue(in_color))), (int(amplitude * RGBGetRValue(in_color))));
    for (int x = 0; x < 256; x++)
    {
        if (frgd_mode >= VISUALIZER_PATTERN_ANIM_RAINBOW_SINUSOIDAL)
        {
            in_color = fg_pixels->pixels[ROW_IDX_SINGLE_COLOR][x];
            out_color = ToRGBColor((int(amplitude * RGBGetBValue(in_color))), (int(amplitude * RGBGetGValue(in_color))), (int(amplitude * RGBGetRValue(in_color))));
        }

        out_pixels->pixels[ROW_IDX_SINGLE_COLOR][x] = out_color;
    }
}

void AudioVisualizer::DrawSingleColorBackground(float amplitude, vis_pixels *bg_pixels, vis_pixels *out_pixels)
{
    if (amplitude >= 1.0f)
    {
        amplitude = 1.0f;
    }
    else if (amplitude <= 0.0f)
    {
        amplitude = 0.0f;
    }

    for (int x = 0; x < 256; x++)
    {
        int in_color = bg_pixels->pixels[ROW_IDX_SINGLE_COLOR][x];
        int out_color = ToRGBColor((int(amplitude * RGBGetBValue(in_color))), (int(amplitude * RGBGetGValue(in_color))), (int(amplitude * RGBGetRValue(in_color))));
        out_pixels->pixels[ROW_IDX_SINGLE_COLOR][x] = out_color;
    }
}

void AudioVisualizer::DrawSpectrumCycle(int bright, float bkgd_step, vis_pixels *pixels)
{
    bright = (int)(bright * (255.0f / 100.0f));
    hsv_t hsv2 = { 0, 0, 0 };
    hsv2.hue = (int)bkgd_step;
    hsv2.saturation = 255;
    hsv2.value = (unsigned char)bright;
    RGBColor color = hsv2rgb(&hsv2);

    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            pixels->pixels[y][x] = color;
        }
    }
}

void AudioVisualizer::DrawSinusoidalCycle(int bright, float bkgd_step, vis_pixels *pixels)
{
    RGBColor color;
    bright = (int)(bright * (255.0f / 100.0f));
    int red = (int)(127 * (sin(((((int)(((360 / 255.0f)) - bkgd_step) % 360) / 360.0f) * 2 * 3.14f)) + 1));
    int grn = (int)(127 * (sin(((((int)(((360 / 255.0f)) - bkgd_step) % 360) / 360.0f) * 2 * 3.14f) - (6.28f / 3)) + 1));
    int blu = (int)(127 * (sin(((((int)(((360 / 255.0f)) - bkgd_step) % 360) / 360.0f) * 2 * 3.14f) + (6.28f / 3)) + 1));
    color = ToRGBColor(((bright * red) / 256), ((bright * grn) / 256), ((bright * blu) / 256));

    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            pixels->pixels[y][x] = color;
        }
    }
}

void AudioVisualizer::DrawRainbow(int bright, float bkgd_step, vis_pixels *pixels)
{
    bright = (int)(bright * (255.0f / 100.0f));
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            int hsv_h = ((int)(bkgd_step + (256 - x)) % 360);
            hsv_t hsv = { 0, 0, 0 };
            hsv.hue = hsv_h;
            hsv.saturation = 255;
            hsv.value = (unsigned char)bright;
            pixels->pixels[y][x] = hsv2rgb(&hsv);
        }
    }
}

void AudioVisualizer::DrawRainbowSinusoidal(int bright, float bkgd_step, vis_pixels *pixels)
{
    bright = (int)(bright * (255.0f / 100.0f));
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            int red = (int)(127 * (sin(((((int)((x * (360 / 255.0f)) - bkgd_step) % 360) / 360.0f) * 2 * 3.14f)) + 1));
            int grn = (int)(127 * (sin(((((int)((x * (360 / 255.0f)) - bkgd_step) % 360) / 360.0f) * 2 * 3.14f) - (6.28f / 3)) + 1));
            int blu = (int)(127 * (sin(((((int)((x * (360 / 255.0f)) - bkgd_step) % 360) / 360.0f) * 2 * 3.14f) + (6.28f / 3)) + 1));
            pixels->pixels[y][x] = ToRGBColor(((bright * red) / 256), ((bright * grn) / 256), ((bright * blu) / 256));
        }
    }
}

void AudioVisualizer::DrawColorWheel(int bright, float bkgd_step, int center_x, int center_y, vis_pixels *pixels)
{
    bright = (int)(bright * (255.0f / 100.0f));
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            float hue = (float)(bkgd_step + (int)(180 + atan2(y - center_y, x - center_x) * (180.0 / 3.14159)) % 360);
            hsv_t hsv2 = { 0, 0, 0 };
            hsv2.hue = (int)hue;
            hsv2.saturation = 255;
            hsv2.value = (unsigned char)bright;
            pixels->pixels[y][x] = hsv2rgb(&hsv2);
        }
    }
}

void AudioVisualizer::DrawVerticalBars(int bright, RGBColor * colors, int num_colors, vis_pixels *pixels)
{
    bright = (int)(bright * (255.0f / 100.0f));
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            int idx = (int)((float)x * ((float)num_colors / 255.0f));
            pixels->pixels[y][x] = ToRGBColor((int(bright * RGBGetBValue(colors[idx])) / 256), (int(bright * RGBGetGValue(colors[idx])) / 256), (int(bright * RGBGetRValue(colors[idx])) / 256));
        }
    }
}

void AudioVisualizer::DrawHorizontalBars(int bright, RGBColor * colors, int num_colors, vis_pixels *pixels)
{
    bright = (int)(bright * (255.0f / 100.0f));
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            if (y == ROW_IDX_BAR_GRAPH)
            {
                if (x < 128)
                {
                    int idx = (int)(num_colors - ((float)x * ((float)num_colors / 128.0f)));
                    if (idx >= num_colors)
                    {
                        idx = num_colors - 1;
                    }
                    pixels->pixels[y][x] = ToRGBColor(((bright * RGBGetBValue(colors[idx])) / 256), ((bright * RGBGetGValue(colors[idx])) / 256), ((bright * RGBGetRValue(colors[idx])) / 256));
                }
                else
                {
                    int idx = (int)(((float)(x - 128) * ((float)num_colors / 128.0f)));
                    pixels->pixels[y][x] = ToRGBColor(((bright * RGBGetBValue(colors[idx])) / 256), ((bright * RGBGetGValue(colors[idx])) / 256), ((bright * RGBGetRValue(colors[idx])) / 256));
                }
            }
            else
            {
                int idx = (int)(num_colors - ((float)y * ((float)num_colors / 63.0f)));
                pixels->pixels[y][x] = ToRGBColor(((bright * RGBGetBValue(colors[idx])) / 256), ((bright * RGBGetGValue(colors[idx])) / 256), ((bright * RGBGetRValue(colors[idx])) / 256));
            }
        }
    }
}

void AudioVisualizer::DrawPattern(VISUALIZER_PATTERN pattern, int bright, vis_pixels *pixels)
{
    switch (pattern)
    {
    case VISUALIZER_PATTERN_SOLID_BLACK:
        DrawSolidColor(bright, COLOR_BLACK, pixels);
        break;

    case VISUALIZER_PATTERN_SOLID_WHITE:
        DrawSolidColor(bright, COLOR_WHITE, pixels);
        break;

    case VISUALIZER_PATTERN_SOLID_RED:
        DrawSolidColor(bright, COLOR_RED, pixels);
        break;

    case VISUALIZER_PATTERN_SOLID_ORANGE:
        DrawSolidColor(bright, COLOR_ORANGE, pixels);
        break;

    case VISUALIZER_PATTERN_SOLID_YELLOW:
        DrawSolidColor(bright, COLOR_YELLOW, pixels);
        break;

    case VISUALIZER_PATTERN_SOLID_GREEN:
        DrawSolidColor(bright, COLOR_LIME, pixels);
        break;

    case VISUALIZER_PATTERN_SOLID_CYAN:
        DrawSolidColor(bright, COLOR_CYAN, pixels);
        break;

    case VISUALIZER_PATTERN_SOLID_BLUE:
        DrawSolidColor(bright, COLOR_BLUE, pixels);
        break;

    case VISUALIZER_PATTERN_SOLID_PURPLE:
        DrawSolidColor(bright, COLOR_PURPLE, pixels);
        break;

    case VISUALIZER_PATTERN_SOLID_ELECTRIC_AQUAMARINE:
        DrawSolidColor(bright, COLOR_ELECTRIC_ULTRAMARINE, pixels);
        break;

    case VISUALIZER_PATTERN_STATIC_RED_BLUE:
    {
        RGBColor colors[] = { COLOR_RED, COLOR_BLUE };
        DrawHorizontalBars(bright, colors, 2, pixels);
    }
        break;

    case VISUALIZER_PATTERN_STATIC_CYAN_ORANGE:
    {
        RGBColor colors[] = { COLOR_CYAN, COLOR_ORANGE };
        DrawHorizontalBars(bright, colors, 2, pixels);
    }
        break;

    case VISUALIZER_PATTERN_STATIC_CYAN_PURPLE:
    {
        RGBColor colors[] = { COLOR_CYAN, COLOR_PURPLE };
        DrawHorizontalBars(bright, colors, 2, pixels);
    }
        break;

    case VISUALIZER_PATTERN_STATIC_CYAN_ELECTRIC_AQUAMARINE:
    {
        RGBColor colors[] = { COLOR_CYAN, COLOR_ELECTRIC_ULTRAMARINE };
        DrawHorizontalBars(bright, colors, 2, pixels);
    }
        break;

    case VISUALIZER_PATTERN_STATIC_GREEN_YELLOW_RED:
    {
        RGBColor colors[] = { COLOR_LIME, COLOR_YELLOW, COLOR_RED };
        DrawHorizontalBars(bright, colors, 3, pixels);
    }
        break;

    case VISUALIZER_PATTERN_STATIC_GREEN_WHITE_RED:
    {
        RGBColor colors[] = { COLOR_LIME, COLOR_WHITE, COLOR_RED };
        DrawHorizontalBars(bright, colors, 3, pixels);
    }
        break;

    case VISUALIZER_PATTERN_STATIC_BLUE_CYAN_WHITE:
    {
        RGBColor colors[] = { COLOR_BLUE, COLOR_CYAN, COLOR_WHITE };
        DrawHorizontalBars(bright, colors, 3, pixels);
    }
        break;

    case VISUALIZER_PATTERN_STATIC_RED_WHITE_BLUE:
    {
        RGBColor colors[] = { COLOR_RED, COLOR_WHITE, COLOR_BLUE };
        DrawHorizontalBars(bright, colors, 3, pixels);
    }
        break;

    case VISUALIZER_PATTERN_STATIC_RAINBOW:
    {
        RGBColor colors[] = { COLOR_RED, COLOR_YELLOW, COLOR_LIME, COLOR_CYAN, COLOR_BLUE, COLOR_PURPLE };
        DrawHorizontalBars(bright, colors, 6, pixels);
    }
        break;

    case VISUALIZER_PATTERN_STATIC_RAINBOW_INVERSE:
    {
        RGBColor colors[] = { COLOR_PURPLE, COLOR_BLUE, COLOR_CYAN, COLOR_LIME, COLOR_YELLOW, COLOR_RED };
        DrawHorizontalBars(bright, colors, 6, pixels);
    }
        break;

    case VISUALIZER_PATTERN_ANIM_RAINBOW_SINUSOIDAL:
        DrawRainbowSinusoidal(bright, bkgd_step, pixels);
        break;

    case VISUALIZER_PATTERN_ANIM_RAINBOW_HSV:
        DrawRainbow(bright, bkgd_step, pixels);
        break;

    case VISUALIZER_PATTERN_ANIM_COLOR_WHEEL:
        DrawColorWheel(bright, bkgd_step, 128, 32, pixels);
        break;

    case VISUALIZER_PATTERN_ANIM_COLOR_WHEEL_2:
        DrawColorWheel(bright, bkgd_step, 128, 64, pixels);
        break;

    case VISUALIZER_PATTERN_ANIM_SPECTRUM_CYCLE:
        DrawSpectrumCycle(bright, bkgd_step, pixels);
        break;

    case VISUALIZER_PATTERN_ANIM_SINUSOIDAL_CYCLE:
        DrawSinusoidalCycle(bright, bkgd_step, pixels);
        break;
    }
}

void AudioVisualizer::SetupMatrixGrid(int x_count, int y_count, int * x_idx_list, int * y_idx_list)
{
    for(int x = 0; x < x_count; x++)
    {
        if(x_count < 10)
        {
            x_idx_list[x] = (int)((x * (SPECTROGRAPH_COLS / (float)(x_count))) + (0.5f * (SPECTROGRAPH_COLS / (float)(x_count))));
        }
        else if(x < ((x_count) / 2))
        {
            x_idx_list[x] = (int)((x * (SPECTROGRAPH_COLS / (float)(x_count - 1))) + (0.5f * (SPECTROGRAPH_COLS / (float)(x_count - 1))));
        }
        else
        {
            x_idx_list[x] = (int)((x * (SPECTROGRAPH_COLS / (float)(x_count - 1))) - (0.5f * (SPECTROGRAPH_COLS / (float)(x_count - 1))));
        }

    }

    for(int y = 0; y < y_count; y++)
    {
        y_idx_list[y] = (int)(ROW_IDX_SPECTROGRAPH_TOP + (y * (SPECTROGRAPH_ROWS / (float)y_count)) + (0.5f * (SPECTROGRAPH_ROWS / (float)y_count)));
    }
}

void AudioVisualizer::SetupLinearGrid(int x_count, int * x_idx_list)
{
    if((x_count % 2) == 0)
    {
        /*--------------------*\
        | Even number of LEDs  |
        \*--------------------*/
        for(int x = 0; x < x_count; x++)
        {
            x_idx_list[x] = (int)((float)x * (256.0f / (float)x_count)) + (128.0f / (float)x_count);
        }
    }
    else
    {
        /*-------------------*\
        | Odd number of LEDs  |
        \*-------------------*/
        for(int x = 0; x < x_count; x++)
        {
            if (x == (x_count / 2))
            {
                x_idx_list[x] = 128;
            }
            else if (x < ((x_count / 2) + 1))
            {
                x_idx_list[x] = (x_count / 2) + ((x + 1) * (256 / (x_count + 1)));
            }
            else
            {
                x_idx_list[x] = ((x_count / 2) + 1) + (x * (256 / (x_count + 1)));
            }
        }
    }
}

/*------------------------------*\
| GUI Bindings                   |
\*------------------------------*/
void AudioVisualizer::on_audio_settings_clicked()
{
    audio_settings.show();
}

void AudioVisualizer::on_comboBox_Background_Mode_currentIndexChanged(int index)
{
    bkgd_mode = index;
}

void AudioVisualizer::on_comboBox_Foreground_Mode_currentIndexChanged(int index)
{
    frgd_mode = index;
}

void AudioVisualizer::on_comboBox_Single_Color_Mode_currentIndexChanged(int index)
{
    single_color_mode = index;
}

void AudioVisualizer::on_checkBox_Reactive_Background_stateChanged(int value)
{
    reactive_bkgd = value;

    if (reactive_bkgd)
    {
        ui->checkBox_Silent_Background->setChecked(false);
    }
}

void AudioVisualizer::on_checkBox_Silent_Background_stateChanged(int value)
{
    silent_bkgd = value;

    if (silent_bkgd)
    {
        ui->checkBox_Reactive_Background->setChecked(false);
    }
}

void AudioVisualizer::on_doubleSpinBox_Background_Timeout_valueChanged(double value)
{
    background_timeout = value;
    background_timer = 0;
}

/*----------------------------------------------*\
| From/to json                                   |
\*----------------------------------------------*/
void AudioVisualizer::LoadCustomSettings(json Settings)
{
    if (Settings.contains("BackgroundBrightness"))
        ui->spinBox_Background_Brightness->setValue(Settings["BackgroundBrightness"]);

    if (Settings.contains("BackgroundMode"))
        ui->comboBox_Background_Mode->setCurrentIndex(Settings["BackgroundMode"]);

    if (Settings.contains("ForegroundMode"))
        ui->comboBox_Foreground_Mode->setCurrentIndex(Settings["ForegroundMode"]);

    if (Settings.contains("SingleColorMode"))
        ui->comboBox_Single_Color_Mode->setCurrentIndex(Settings["SingleColorMode"]);

    if (Settings.contains("AnimationSpeed"))
        ui->doubleSpinBox_Animation_Speed->setValue(Settings["AnimationSpeed"]);

    if (Settings.contains("ReactiveBackground"))
        ui->checkBox_Reactive_Background->setChecked(Settings["ReactiveBackground"]);

    if (Settings.contains("SilentBackground"))
        ui->checkBox_Silent_Background->setChecked(Settings["SilentBackground"]);

    if (Settings.contains("BackgroundTimeout"))
        ui->doubleSpinBox_Background_Timeout->setValue(Settings["BackgroundTimeout"]);
}

json AudioVisualizer::SaveCustomSettings()
{
    json Settings;

    Settings["BackgroundBrightness"] = bkgd_bright;
    Settings["BackgroundMode"] = bkgd_mode;
    Settings["ForegroundMode"] = frgd_mode;
    Settings["SingleColorMode"] = single_color_mode;
    Settings["AnimationSpeed"] = anim_speed;
    Settings["ReactiveBackground"] = reactive_bkgd;
    Settings["SilentBackground"] = silent_bkgd;
    Settings["BackgroundTimeout"] = background_timeout;
    Settings["audio_settings"] = audio_settings_struct;

    return Settings;
}
