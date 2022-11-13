#include "Sequence.h"
#include "ColorUtils.h"

REGISTER_EFFECT(Sequence);

Sequence::Sequence(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::Sequence)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "Sequence";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Alternates colors with a fade effect";
    EffectDetails.MaxSpeed     = 20;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.MaxSlider2Val = 100;
    EffectDetails.MinSlider2Val = 1;
    EffectDetails.Slider2Name   = "Fade time";
    EffectDetails.HasCustomSettings = true;

    SetSpeed(10);
}

Sequence::~Sequence()
{
    delete ui;
}

void Sequence::DefineExtraOptions(QLayout* layout)
{
    layout->addWidget(this);
}

void Sequence::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    std::vector<RGBColor> colors = ui->colorsPicker->Colors();
    unsigned int colors_count = colors.size();
    unsigned int current_color_index = ((int)ceil(progress)) % colors_count;

    float whole;
    float frac = std::modf(progress, &whole);

    float fade_mult;

    RGBColor color;

    if(frac >= 0.8)
    {
       unsigned int next_color_index = current_color_index < colors_count - 1 ? current_color_index + 1 : 0;
       color = ColorUtils::Interpolate(colors[current_color_index], colors[next_color_index], (frac - 0.8) * 5);
       fade_mult = 1.f / (float) Slider2Val;
    }
    else
    {
        color = ui->colorsPicker->Colors()[current_color_index];
        fade_mult = 1.f;
    }

    for(ControllerZone* controller_zone : controller_zones)
    {
        controller_zone->SetAllZoneLEDs(color, Brightness);
    }

    progress += fade_mult * 0.1 * (float) Speed / (float) FPS;
}


void Sequence::LoadCustomSettings(json settings)
{
    if (settings.contains("colors"))
    {
        ui->colorsPicker->SetColors(settings["colors"]);
    }
}

json Sequence::SaveCustomSettings()
{
    json settings;

    settings["colors"] = ui->colorsPicker->Colors();

    return settings;
}
