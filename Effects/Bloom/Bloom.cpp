#include "Bloom.h"
#include "ColorUtils.h"

REGISTER_EFFECT(Bloom);

Bloom::Bloom(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::Bloom)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "Bloom";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Flower blooming effect";
    EffectDetails.HasCustomSettings = true;
    EffectDetails.SupportsRandom = false;

    EffectDetails.MaxSpeed     = 200;
    EffectDetails.MinSpeed     = 10;

    SetSpeed(100);
}

Bloom::~Bloom()
{
    delete ui;
}

void Bloom::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    for(unsigned int i = 0; i < controller_zones.size(); i++)
    {
        if(flowers[i].size() != controller_zones[i]->size())
        {
            Reset(controller_zones);
        }

        UpdateFlowers(i);

        zone_type ZT = controller_zones[i]->type();
        unsigned int leds_count = controller_zones[i]->leds_count();

        if (ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            for (unsigned int LedID = 0; LedID < leds_count; LedID++)
            {
                RGBColor color = hsv2rgb(&flowers[i][LedID].hsv);
                controller_zones[i]->SetLED(LedID, color, Brightness);
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
                   int idx = (row_id * cols) + col_id;
                   int LedID = controller_zones[i]->map()[idx];
                   RGBColor color = hsv2rgb(&flowers[i][idx].hsv);
                   controller_zones[i]->SetLED(LedID, color, Brightness);
                }
            }
        }
    }
}

void Bloom::on_saturation_valueChanged(int value)
{
    saturation = value;
}

void Bloom::UpdateFlowers(unsigned int i)
{
    float delta = (float) Speed / (float) FPS;

    for(Flower& flower: flowers[i])
    {
        flower.hue += (flower.speed_mult * delta);
        flower.hsv.hue = (int) flower.hue % 360;
        flower.hsv.saturation = saturation;
    }
}

void Bloom::Reset(std::vector<ControllerZone*> controller_zones)
{
    flowers.clear();

    for(ControllerZone* controller_zone: controller_zones)
    {
        std::vector<Flower> zone_flowers;

        for(unsigned int i = 0; i < controller_zone->size(); i++)
        {
            Flower flower;
            flower.hsv = ColorUtils::RandomHSVColor();
            flower.hue = flower.hsv.hue;
            flower.saturation = flower.hsv.saturation;
            flower.speed_mult = ((double) rand() / (RAND_MAX)) + 1;
            zone_flowers.push_back(flower);
        }

        flowers.push_back(zone_flowers);
    }
}

void Bloom::OnControllerZonesListChanged(std::vector<ControllerZone*> controller_zones)
{
    Reset(controller_zones);
}

json Bloom::SaveCustomSettings()
{
    json settings;

    settings["saturation"] = saturation;

    return settings;
}

void Bloom::LoadCustomSettings(json Settings)
{
    if (Settings.contains("saturation"))
        ui->saturation->setValue(Settings["saturation"]);
}
