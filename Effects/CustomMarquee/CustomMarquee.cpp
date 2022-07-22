#include "CustomMarquee.h"
#include "ColorUtils.h"

REGISTER_EFFECT(CustomMarquee);

CustomMarquee::CustomMarquee(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::CustomMarquee)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "CustomMarquee";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Create your own marquee effect";
    EffectDetails.IsReversable = true;
    EffectDetails.MaxSpeed     = 50;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.HasCustomSettings = true;

    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->colors->setLayout(new QHBoxLayout());
    ui->colors->layout()->setSizeConstraint(QLayout::SetFixedSize);
    ui->scrollArea->setWidgetResizable(true);

    SetSpeed(25);

    ResetColors();
}

CustomMarquee::~CustomMarquee()
{
    delete ui;
}

void CustomMarquee::DefineExtraOptions(QLayout* layout)
{
    layout->addWidget(this);
}

void CustomMarquee::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    for(unsigned int i = 0; i < controller_zones.size(); i++)
    {
        int start_idx = controller_zones[i]->start_idx();
        zone_type ZT = controller_zones[i]->type();
        int leds_count = controller_zones[i]->leds_count();
        bool reverse = controller_zones[i]->reverse;

        if (ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            for (int LedID = 0; LedID < leds_count; LedID++)
            {
                RGBColor color = GetColor(reverse ? leds_count - LedID - 1 : LedID);
                controller_zones[i]->SetLED(start_idx + LedID, color, Brightness);
            }
        }

        else if (ZT == ZONE_TYPE_MATRIX)
        {
            int cols = controller_zones[i]->matrix_map_width();
            int rows = controller_zones[i]->matrix_map_height();

            for (int col_id = 0; col_id < cols; col_id++)
            {
                RGBColor color = GetColor(reverse ? cols - col_id - 1: col_id);

                for (int row_id = 0; row_id < rows; row_id++)
                {
                    int LedID = controller_zones[i]->controller->zones[controller_zones[i]->zone_idx].matrix_map->map[((row_id * cols) + col_id)];
                    controller_zones[i]->SetLED(start_idx + LedID, color, Brightness);
                }
            }

        }
    }

    progress += Speed / (float) FPS;
}

RGBColor CustomMarquee::GetColor(unsigned int i)
{
    int index = (i + (int) progress) % colors.size();
    return colors[index];
}

ColorPicker* CustomMarquee::CreatePicker(int i)
{
    ColorPicker* picker = new ColorPicker();
    picker->SetRGBColor(colors[i]);

    color_pickers[i] = picker;

    connect(picker, &ColorPicker::ColorSelected, [=](QColor c){
        colors[i] = ColorUtils::fromQColor(c);
    });

    return picker;
}

void CustomMarquee::ResetColors()
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

void CustomMarquee::on_colors_count_spinBox_valueChanged(int)
{
    ResetColors();
}

void CustomMarquee::LoadCustomSettings(json settings)
{
    if (settings.contains("colors"))
    {
        colors.clear();

        for(unsigned int color : settings["colors"])
        {
            colors.push_back(color);
        }

        ui->colors_count_spinBox->setValue(colors.size());
    }

    ResetColors();
}

json CustomMarquee::SaveCustomSettings(json settings)
{
    settings["colors"] = colors;
    return settings;
}
