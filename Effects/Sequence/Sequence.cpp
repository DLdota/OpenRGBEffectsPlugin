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

    EffectDetails.IsReversable = false;
    EffectDetails.MaxSpeed     = 20;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.UserColors   = 0;

    EffectDetails.MaxSlider2Val = 100;
    EffectDetails.MinSlider2Val = 1;
    EffectDetails.Slider2Name   = "Fade time";

    EffectDetails.HasCustomWidgets = true;
    EffectDetails.HasCustomSettings = true;

    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->colors->setLayout(new QHBoxLayout());
    ui->colors->layout()->setSizeConstraint(QLayout::SetFixedSize);
    ui->scrollArea->setWidgetResizable(true);

    ResetColors();

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
    unsigned int colors_count = ui->colors_count_spinBox->value();
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
        color = colors[current_color_index];
        fade_mult = 1.f;
    }

    for(ControllerZone* controller_zone : controller_zones)
    {
        controller_zone->SetAllZoneLEDs(color, Brightness);
    }

    progress += fade_mult * 0.1 * (float) Speed / (float) FPS;
}

ColorPicker* Sequence::CreatePicker(int i)
{
    ColorPicker* picker = new ColorPicker();
    picker->SetRGBColor(colors[i]);

    color_pickers[i] = picker;

    connect(picker, &ColorPicker::ColorSelected, [=](QColor c){
        colors[i] = ColorUtils::fromQColor(c);
    });

    return picker;
}

void Sequence::ResetColors()
{
    QLayoutItem *child;

    while ((child = ui->colors->layout()->takeAt(0)) != 0) {
        delete child->widget();
    }

    unsigned int colors_count = ui->colors_count_spinBox->value();

    color_pickers.resize(colors_count);
    colors.resize(colors_count);

    for(unsigned int i = 0; i < colors_count; i++)
    {
        ColorPicker* picker = CreatePicker(i);
        ui->colors->layout()->addWidget(picker);
    }

}

void Sequence::LoadCustomSettings(json Settings)
{
    if (Settings.contains("colors"))
    {
        colors.clear();

        for(unsigned int color : Settings["colors"])
        {
            colors.push_back(color);
        }

        ui->colors_count_spinBox->setValue(colors.size());
    }

    ResetColors();
}

json Sequence::SaveCustomSettings(json Settings)
{
    Settings["colors"] = colors;
    return Settings;
}

void Sequence::on_colors_count_spinBox_valueChanged(int)
{
    ResetColors();
}
