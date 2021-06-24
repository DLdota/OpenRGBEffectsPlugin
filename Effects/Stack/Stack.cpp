#include "Stack.h"
#include "hsv.h"
#include "ColorUtils.h"

REGISTER_EFFECT(Stack);

Stack::Stack(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Stack)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "Stack";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Fills and stack the zones";

    EffectDetails.IsReversable = true;
    EffectDetails.MaxSpeed     = 20;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.UserColors   = 1;
    EffectDetails.AllowOnlyFirst = false;

    EffectDetails.MaxSlider2Val = 0;
    EffectDetails.MinSlider2Val = 0;
    EffectDetails.Slider2Name   = "";

    EffectDetails.HasCustomWidgets = true;
    EffectDetails.HasCustomSettings = true;

    ui->direction->addItems({"Horizontal", "Vertical"});
}

Stack::~Stack()
{
    delete ui;
}


void Stack::DefineExtraOptions(QLayout* layout)
{
    layout->addWidget(this);
}

void Stack::StepEffect(std::vector<ControllerZone> controller_zones)
{    
    unsigned int size = controller_zones.size();

    if(reset)
    {
        for(unsigned int i = 0; i < size; i++)
        {
            ResetZone(i, controller_zones[i]);
        }
        reset = false;
    }


    for(unsigned int i = 0; i < controller_zones.size(); i++)
    {
        int start_idx = controller_zones[i].start_idx();
        zone_type ZT = controller_zones[i].type();
        int leds_count = controller_zones[i].leds_count();
        bool reverse = controller_zones[i].reverse;

        if (ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            for (int LedID = 0; LedID < leds_count; LedID++)
            {
                RGBColor color = GetColor(i, reverse ? leds_count - LedID - 1 : LedID);
                current_colors[i][LedID] = color;
                controller_zones[i].controller->SetLED(start_idx + LedID, color);
            }
        }

        else if (ZT == ZONE_TYPE_MATRIX)
        {
            int cols = controller_zones[i].matrix_map_width();
            int rows = controller_zones[i].matrix_map_height();

            // horizontal
            if(ui->direction->currentIndex() == 0)
            {
                for (int col_id = 0; col_id < cols; col_id++)
                {
                    RGBColor color = GetColor(i, reverse ? cols - col_id - 1: col_id);
                    current_colors[i][col_id] = color;

                    for (int row_id = 0; row_id < rows; row_id++)
                    {
                        int LedID = controller_zones[i].controller->zones[controller_zones[i].zone_idx].matrix_map->map[((row_id * cols) + col_id)];
                        controller_zones[i].controller->SetLED(start_idx + LedID, color);
                    }
                }
            }
            // vertical
            else
            {
                for (int row_id = 0; row_id < rows; row_id++)
                {
                    RGBColor color = GetColor(i, reverse ? rows - row_id - 1: row_id);
                    current_colors[i][row_id] = color;

                    for (int col_id = 0; col_id < cols; col_id++)
                    {
                        int LedID = controller_zones[i].controller->zones[controller_zones[i].zone_idx].matrix_map->map[((row_id * cols) + col_id)];
                        controller_zones[i].controller->SetLED(start_idx + LedID, color);
                    }
                }
            }
        }
    }

    float delta_progress = 0.1 * Speed / (float) FPS;

    for(unsigned int i = 0; i < size; i++)
    {
        progress[i] += delta_progress * current_colors[i].size();

        if(progress[i] >= stops[i])
        {
            stops[i]--;

            if(stops[i] == 0)
            {
                ResetZone(i, controller_zones[i]);
            }

            progress[i] = 0.f;
        }
    }
}

RGBColor Stack::GetColor(unsigned int controller_zone_idx, unsigned int led_idx)
{
    unsigned int stop = stops[controller_zone_idx];

    if(stop < led_idx)
    {
        return zone_colors[controller_zone_idx];
    }

    float zone_progress = progress[controller_zone_idx];

    float distance = fabs(zone_progress - led_idx);

    if(distance > 1)
    {
        return ColorUtils::OFF();
    }

    return ColorUtils::Enlight(zone_colors[controller_zone_idx], 1 - distance);
}

void Stack::ASelectionWasChanged(std::vector<ControllerZone> controller_zones)
{
    stops.clear();
    progress.clear();
    current_colors.clear();
    zone_colors.clear();

    unsigned int size = controller_zones.size();

    stops.resize(size);
    progress.resize(size);
    current_colors.resize(size);
    zone_colors.resize(size);

    for(unsigned int i = 0; i < size; i++)
    {
        ResetZone(i, controller_zones[i]);
    }
}

void Stack::ResetZone(unsigned int controller_zone_idx, ControllerZone controller_zone)
{
    unsigned int leds_count;

    if(controller_zone.type() == ZONE_TYPE_MATRIX)
    {
        if(ui->direction->currentIndex() == 0)
        {
            leds_count = controller_zone.matrix_map_width();
        }
        else
        {
            leds_count = controller_zone.matrix_map_height();
        }
    }
    else
    {
        leds_count = controller_zone.leds_count();
    }

    stops[controller_zone_idx] = std::max<unsigned int>(leds_count - 1, 1);
    progress[controller_zone_idx] = 0.f;
    current_colors[controller_zone_idx].resize(leds_count);

    for(unsigned int c = 0; c < leds_count; c++)
    {
        current_colors[controller_zone_idx][c] = ColorUtils::OFF();
    }

    zone_colors[controller_zone_idx] = RandomColorsEnabled ? ColorUtils::RandomRGBColor() : UserColors[0];
}


void Stack::LoadCustomSettings(json settings)
{
    if(settings.contains("direction"))
    {
        ui->direction->setCurrentIndex(settings["direction"]);
    }
}

json Stack::SaveCustomSettings(json settings)
{
    settings["direction"] = ui->direction->currentIndex();
    return settings;
}

void Stack::on_direction_currentIndexChanged(int)
{
    reset = true;
}


