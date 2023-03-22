#include "Mosaic.h"
#include "ColorUtils.h"

REGISTER_EFFECT(Mosaic);

Mosaic::Mosaic(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::Mosaic)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "Mosaic";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Tiles randomly spawning across your devices";
    EffectDetails.IsReversable = true;
    EffectDetails.MaxSpeed     = 200;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.UserColors   = 2;
    EffectDetails.MaxSlider2Val = 1000;
    EffectDetails.MinSlider2Val = 10;
    EffectDetails.Slider2Name   = "Rarity";

    SetSpeed(10);
}

Mosaic::~Mosaic()
{
    delete ui;
}

void Mosaic::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    for(unsigned int i = 0; i < controller_zones.size(); i++)
    {        
        zone_type ZT = controller_zones[i]->type();

        if(tiles[i].size() != controller_zones[i]->size())
        {
            tiles[i].resize(controller_zones[i]->size());
        }

        UpdateTiles(i);

        if (ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            unsigned int leds_count = controller_zones[i]->leds_count();

            for (unsigned int LedID = 0; LedID < leds_count; LedID++)
            {
                RGBColor color = hsv2rgb(&tiles[i][LedID].hsv);
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
                   RGBColor color = hsv2rgb(&tiles[i][idx].hsv);
                   controller_zones[i]->SetLED(LedID, color, Brightness);
                }
            }

        }

    }
}

void Mosaic::UpdateTiles(unsigned int controller_zone_idx)
{
    unsigned int rarity = Slider2Val;

    for(Tile& tile: tiles[controller_zone_idx])
    {
        if(tile.brightness <= 0.f)
        {
            if(rand() % rarity == 0)
            {
                tile.brightness = 1.f;
                tile.decrease_speed_mult = ((double) rand() / (RAND_MAX)) + 1;

                if(RandomColorsEnabled)
                {
                    tile.hsv = ColorUtils::RandomHSVColor();
                }
                else
                {
                    rgb2hsv(UserColors[rand() % UserColors.size()], &tile.hsv);
                }
            }
        }

        tile.brightness -= 0.0005 * Speed * tile.decrease_speed_mult;

        tile.hsv.value = tile.brightness > 0 ?tile.brightness * 255 : 0;
    }
}

void Mosaic::ResetMosaic(std::vector<ControllerZone*> controller_zones)
{
    tiles.clear();

    for(ControllerZone* controller_zone: controller_zones)
    {
        std::vector<Tile> zone_tiles;        
        zone_tiles.resize(controller_zone->size());
        tiles.push_back(zone_tiles);
    }
}

void Mosaic::OnControllerZonesListChanged(std::vector<ControllerZone*> controller_zones)
{
    ResetMosaic(controller_zones);
}
