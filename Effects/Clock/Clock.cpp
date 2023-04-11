#include "Clock.h"

REGISTER_EFFECT(Clock);

Clock::Clock(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::Clock)
{
    ui->setupUi(this);

    EffectDetails.EffectName = ClassName();
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Digital clock";
    EffectDetails.UserColors = 3;
    EffectDetails.SupportsRandom = false;
}

Clock::~Clock()
{
    delete ui;
}

#ifdef WIN32
#define localtime_r(_Time, _Tm) localtime_s(_Tm, _Time)
#endif

void Clock::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    tm localTime;
    std::chrono::system_clock::time_point t = std::chrono::system_clock::now();
    time_t now = std::chrono::system_clock::to_time_t(t);
    localtime_r(&now, &localTime);

    const std::chrono::duration<double> tse = t.time_since_epoch();
    std::chrono::seconds::rep milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(tse).count() % 1000;

    s = localTime.tm_sec + milliseconds * 0.001;
    m = localTime.tm_min + localTime.tm_sec / 60.;
    h = (localTime.tm_hour % 12) + localTime.tm_min / 60.;

    for(ControllerZone* controller_zone : controller_zones)
    {
        zone_type ZT = controller_zone->type();
        bool reverse = controller_zone->reverse;

        if(ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            unsigned int width = controller_zone->leds_count();

            for(unsigned int i = 0; i < width; i++)
            {
                RGBColor color = GetColor(reverse ? width - i - 1 : i, width);
                controller_zone->SetLED(i, ColorUtils::fromQColor(color), Brightness, Temperature, Tint);
            }

        }
        else if(ZT == ZONE_TYPE_MATRIX)
        {
            int cols = controller_zone->matrix_map_width();
            int rows = controller_zone->matrix_map_height();

            for (int col_id = 0; col_id < cols; col_id++)
            {
                RGBColor color = GetColor(reverse ? cols - col_id - 1: col_id, cols);

                for (int row_id = 0; row_id < rows; row_id++)
                {
                    int LedID = controller_zone->map()[((row_id * cols) + col_id)];
                    controller_zone->SetLED(LedID, color, Brightness, Temperature, Tint);
                }
            }
        }
    }
}

RGBColor Clock::GetColor(float x, float w)
{
    float step_h = (w-1) * h / 12.0; // [0-w]
    float step_m = (w-1) * m / 60.0; // [0-w]
    float step_s = (w-1) * s / 60.0; // [0-w]

    RGBColor hours_coulor = 0;
    RGBColor minutes_coulor = 0;
    RGBColor seconds_coulor = 0;

    if(fabs(x - step_h) <= 1)
    {
        if(x < step_h)
        {
            hours_coulor = ColorUtils::apply_brightness(UserColors[0], 1-(step_h - x));
        }
        else
        {
            hours_coulor = ColorUtils::apply_brightness(UserColors[0], 1-(x - step_h));
        }
    }

    if(fabs(x - step_m) <= 1)
    {
        if(x < step_m)
        {
            minutes_coulor = ColorUtils::apply_brightness(UserColors[1], 1-(step_m - x));
        }
        else
        {
            minutes_coulor = ColorUtils::apply_brightness(UserColors[1], 1-(x - step_m));
        }
    }

    if(fabs(x - step_s) <= 1)
    {
        if(x < step_s)
        {
            seconds_coulor = ColorUtils::apply_brightness(UserColors[2], 1-(step_s - x));
        }
        else
        {
            seconds_coulor = ColorUtils::apply_brightness(UserColors[2], 1-(x - step_s));
        }
    }

    return ColorUtils::Lighten(ColorUtils::Lighten(hours_coulor, minutes_coulor), seconds_coulor);
}
