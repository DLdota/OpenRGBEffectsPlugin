#include "MotionPoints.h"
#include "ColorUtils.h"

REGISTER_EFFECT(MotionPoints);

MotionPoints::MotionPoints(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::MotionPoints)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "MotionPoints";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Multiple points that moves in all directions on your devices";
    EffectDetails.IsReversable = true;
    EffectDetails.MaxSpeed     = 200;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.UserColors   = 2;
    EffectDetails.MaxSlider2Val = 100;
    EffectDetails.MinSlider2Val = 1;
    EffectDetails.Slider2Name   = "Number of points";

    SetSpeed(100);
    RGBEffect::SetSlider2Val(20);
}

MotionPoints::~MotionPoints()
{
    delete ui;
}

void MotionPoints::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    UpdatePoints();

    for(ControllerZone* controller_zone: controller_zones)
    {
        zone_type ZT = controller_zone->type();
        int leds_count = controller_zone->leds_count();

        if (ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            for (int LedID = 0; LedID < leds_count; LedID++)
            {
                RGBColor color = GetColor(LedID, 0, leds_count, 1);
                controller_zone->SetLED(LedID, color, Brightness, Temperature, Tint);
            }
        }

        else if (ZT == ZONE_TYPE_MATRIX)
        {
            int cols = controller_zone->matrix_map_width();
            int rows = controller_zone->matrix_map_height();

            for (int col_id = 0; col_id < cols; col_id++)
            {
                for (int row_id = 0; row_id < rows; row_id++)
                {
                    RGBColor color = GetColor(col_id, row_id, cols, rows);
                    int LedID = controller_zone->map()[((row_id * cols) + col_id)];
                    controller_zone->SetLED(LedID, color, Brightness, Temperature, Tint);
                }
            }

        }
    }
}

void MotionPoints::UpdatePoints()
{
    for(MovingPoint& point: points)
    {
        point.Move(0.0001 * Speed * point.speed_mult);

        if(point.IsOut())
        {
            point.Init(RandomColorsEnabled ? ColorUtils::RandomRGBColor() : UserColors[rand() % UserColors.size()]);
        }
    }
}

RGBColor MotionPoints::GetColor(float x, float y, float w, float h)
{
    for(MovingPoint& point: points)
    {
       float distance = sqrt(pow(point.x * w - x, 2) + pow(point.y * h - y, 2));

       if(distance <= 1)
       {
           return ColorUtils::Enlight(point.color, 1 - distance);
       }
    }

    return ColorUtils::OFF();
}

void MotionPoints::SetSlider2Val(unsigned int value)
{
    Slider2Val = value;
    points.clear();

    UserColors.resize(2);

    for(unsigned int i = 0; i < value; i++)
    {
        MovingPoint point;
        point.Init(RandomColorsEnabled ? ColorUtils::RandomRGBColor() : UserColors[rand() % UserColors.size()]);
        points.push_back(point);
    }
}

