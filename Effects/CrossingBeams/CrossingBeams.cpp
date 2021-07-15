#include "CrossingBeams.h"
#include "hsv.h"
#include "ColorUtils.h"

REGISTER_EFFECT(CrossingBeams);

CrossingBeams::CrossingBeams(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::CrossingBeams)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "CrossingBeams";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Moving beams";

    EffectDetails.IsReversable = false;
    EffectDetails.MaxSpeed     = 10;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.UserColors   = 2;
    EffectDetails.AllowOnlyFirst = false;

    EffectDetails.MaxSlider2Val = 0;
    EffectDetails.MinSlider2Val = 0;
    EffectDetails.Slider2Name   = "";

    EffectDetails.HasCustomWidgets = true;
    EffectDetails.HasCustomSettings = true;

}

CrossingBeams::~CrossingBeams()
{
    delete ui;
}


void CrossingBeams::DefineExtraOptions(QLayout* layout)
{
    layout->addWidget(this);
}


void CrossingBeams::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    sine_x = sin( 0.01 * h_speed * progress);
    sine_y = sin( 0.01 * v_speed *  progress);

    for(unsigned int i = 0; i < controller_zones.size(); i++)
    {
        int start_idx = controller_zones[i]->start_idx();
        zone_type ZT = controller_zones[i]->type();
        int leds_count = controller_zones[i]->leds_count();

        if (ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            for (int LedID = 0; LedID < leds_count; LedID++)
            {
                RGBColor color = GetColor(LedID, 0, leds_count, 1);
                controller_zones[i]->controller->SetLED(start_idx + LedID, color);
            }
        }

        else if (ZT == ZONE_TYPE_MATRIX)
        {
            int cols = controller_zones[i]->matrix_map_width();
            int rows = controller_zones[i]->matrix_map_height();

            for (int col_id = 0; col_id < cols; col_id++)
            {
                for (int row_id = 0; row_id < rows; row_id++)
                {
                    RGBColor color = GetColor(col_id, row_id, cols, rows);
                    int LedID = controller_zones[i]->controller->zones[controller_zones[i]->zone_idx].matrix_map->map[((row_id * cols) + col_id)];
                    controller_zones[i]->controller->SetLED(start_idx + LedID, color);
                }
            }
        }
    }

    progress += (float) Speed / (float) FPS;

    if(RandomColorsEnabled)
    {
        hsv1.hue++;
        hsv2.hue++;
    }
}


RGBColor CrossingBeams::GetColor(unsigned int x, unsigned int y, unsigned int w, unsigned int h){

    float x_progress = 0.5 * (1 + sine_x) * w;
    float y_progress = 0.5 * (1 + sine_y) * h;

    float distance_x = fabs(x_progress - x);
    float distance_x_percent = std::min<float>(1,pow(distance_x / w,
                                                     distance_x > thickness ? 0.01 * glow : 1));

    float distance_y = fabs(y_progress - y);
    float distance_y_percent = std::min<float>(1, pow(distance_y / h,
                                                      distance_y > thickness ? 0.01 * glow : 1));

    hsv_t hsv_tmp;

    hsv_tmp.value = hsv1.value * (1 - distance_x_percent);
    hsv_tmp.hue = hsv1.hue;
    hsv_tmp.saturation = hsv1.saturation;

    RGBColor color1 = RGBColor(hsv2rgb(&hsv_tmp));

    hsv_tmp.value = hsv2.value * (1 - distance_y_percent);
    hsv_tmp.hue = hsv2.hue;
    hsv_tmp.saturation = hsv2.saturation;

    RGBColor color2 = RGBColor(hsv2rgb(&hsv_tmp));

    return ColorUtils::Screen(color1,color2);
}

void CrossingBeams::LoadCustomSettings(json Settings)
{
    if (Settings.contains("v_speed"))    v_speed    = Settings["v_speed"];
    if (Settings.contains("h_speed"))        h_speed        = Settings["h_speed"];
    if (Settings.contains("glow")) glow = Settings["glow"];
    if (Settings.contains("thickness")) thickness = Settings["thickness"];

    ui->v_speed->setValue(v_speed);
    ui->h_speed->setValue(h_speed);
    ui->glow->setValue(glow);
    ui->thickness->setValue(thickness);
}

json CrossingBeams::SaveCustomSettings(json Settings)
{
    Settings["v_speed"]    = v_speed;
    Settings["h_speed"]        = h_speed;
    Settings["glow"] = glow;
    Settings["thickness"] = thickness;
    return Settings;
}

void CrossingBeams::on_v_speed_valueChanged(int value)
{
    v_speed = value;
}

void CrossingBeams::on_h_speed_valueChanged(int value)
{
    h_speed = value;
}

void CrossingBeams::on_glow_valueChanged(int value)
{
    glow = value;
}

void CrossingBeams::on_thickness_valueChanged(int value)
{
    thickness = value;
}

void CrossingBeams::SetUserColors(std::vector<RGBColor> colors)
{
    UserColors = colors;

    ResetUserColors();
}

void CrossingBeams::SetRandomColorsEnabled(bool value)
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


void CrossingBeams::ResetUserColors()
{
    rgb2hsv(UserColors[0], &hsv1);
    rgb2hsv(UserColors[1], &hsv2);
}
