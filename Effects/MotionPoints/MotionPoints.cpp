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
    EffectDetails.EffectDescription = "Multiple points moving effect";

    EffectDetails.IsReversable = true;
    EffectDetails.MaxSpeed     = 20;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.UserColors   = 2;
    EffectDetails.AllowOnlyFirst = false;

    EffectDetails.MaxSlider2Val = 100;
    EffectDetails.MinSlider2Val = 1;
    EffectDetails.Slider2Name   = "Number of points";

    EffectDetails.HasCustomWidgets = false;
    EffectDetails.HasCustomSettings = false;
}

MotionPoints::~MotionPoints()
{
    delete ui;
}

void MotionPoints::DefineExtraOptions(QLayout* layout)
{
    layout->addWidget(this);
}

void MotionPoints::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    UpdatePoints();

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
}

void MotionPoints::UpdatePoints()
{
    for(MovingPoint& point: points)
    {
        point.Move(0.001 * Speed * point.speed_mult);

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

