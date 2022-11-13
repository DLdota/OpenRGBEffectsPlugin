#include "Lightning.h"

REGISTER_EFFECT(Lightning);

Lightning::Lightning(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::Lightning)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "Lightning";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Prepare yourself for thunderstorm";
    EffectDetails.MaxSpeed     = 100;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.UserColors   = 1;
    EffectDetails.MaxSlider2Val = 60;
    EffectDetails.MinSlider2Val = 2;
    EffectDetails.Slider2Name   = "Decay";
    EffectDetails.HasCustomSettings = true;

    SetSpeed(20);
    SetSlider2Val(10);

    ui->lightning_mode->addItems({"Whole strip","Per LED"});
}

Lightning::~Lightning()
{
    delete ui;
}

void Lightning::DefineExtraOptions(QLayout* layout)
{
    layout->addWidget(this);
}

RGBColor Lightning::TriggerLightning(ControllerZone* z, int n)
{
    int Decay = Slider2Val;

    float decrease = 1 + Decay/(float) FPS;

    unsigned int mod = lightning_mode == WHOLE_ZONE ? 1000 : 1000 * z->leds_count();

    Lightnings[z][n].value = ((unsigned int)(rand() % mod)) < Speed ?
                std::min<unsigned char>(255, RandomColorsEnabled ? 255: UserHSV.value) :
                Lightnings[z][n].value > 0 ?  Lightnings[z][n].value / decrease : 0;

    if(RandomColorsEnabled)
    {
        if(Lightnings[z][n].value == 0)
        {
            Lightnings[z][n].hue = rand() % 360;
            Lightnings[z][n].saturation = rand() % 255;
        }
    }
    else
    {
        Lightnings[z][n].hue = UserHSV.hue;
        Lightnings[z][n].saturation = UserHSV.saturation;
    }

    return RGBColor(hsv2rgb(&Lightnings[z][n]));
}

void Lightning::OnControllerZonesListChanged(std::vector<ControllerZone*> controller_zones)
{
    Lightnings.clear();

    for(ControllerZone* controller_zone: controller_zones)
    {
        std::vector<hsv_t> leds;

        for(unsigned int i = 0; i < controller_zone->leds_count(); i++)
        {
            hsv_t lightning;

            lightning.value = 0;
            lightning.hue = UserHSV.hue;
            lightning.saturation = UserHSV.saturation;
            leds.push_back(lightning);
        }

        Lightnings[controller_zone] = leds;
    }
}


void Lightning::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    if(Lightnings.size() != controller_zones.size())
    {
        OnControllerZonesListChanged(controller_zones);
    }

    if(lightning_mode == WHOLE_ZONE)
    {
        for(ControllerZone* controller_zone : controller_zones)
        {
            if(controller_zone->leds_count() > 0)
            {
                RGBColor color = TriggerLightning(controller_zone, 0);
                controller_zone->SetAllZoneLEDs(color, Brightness);
            }
        }
    }
    else if(lightning_mode == PER_LED)
    {
        for(ControllerZone* controller_zone : controller_zones)
        {
            for(unsigned int i = 0; i < controller_zone->leds_count(); i++)
            {
                RGBColor color = TriggerLightning(controller_zone, i);
                controller_zone->SetLED(controller_zone->start_idx() + i, color, Brightness);
            }
        }
    }
}

void Lightning::SetUserColors(std::vector<RGBColor> colors) {
    UserColors = colors;
    rgb2hsv(UserColors[0], &UserHSV);
}

void Lightning::on_lightning_mode_currentIndexChanged(int value)
{
    lightning_mode = static_cast<lightning_mode_value>(value);
}

void Lightning::LoadCustomSettings(json settings)
{
    if (settings.contains("lightning_mode")) ui->lightning_mode->setCurrentIndex(settings["lightning_mode"]);
}

json Lightning::SaveCustomSettings()
{
    json settings;

    settings["lightning_mode"] = lightning_mode;

    return settings;
}
