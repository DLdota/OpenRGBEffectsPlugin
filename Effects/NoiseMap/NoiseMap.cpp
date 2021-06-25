#include "NoiseMap.h"
#include "hsv.h"
#include "ColorUtils.h"
#include <QPainter>
#include <QGradient>

REGISTER_EFFECT(NoiseMap);

NoiseMap::NoiseMap(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::NoiseMap)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "NoiseMap";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Floor is lava";

    EffectDetails.IsReversable = false;
    EffectDetails.MaxSpeed     = 100;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.UserColors   = 0;
    EffectDetails.AllowOnlyFirst = false;

    EffectDetails.MaxSlider2Val = 0;
    EffectDetails.MinSlider2Val = 0;
    EffectDetails.Slider2Name   = "";

    EffectDetails.HasCustomWidgets = true;
    EffectDetails.HasCustomSettings = true;

    ui->colors_choice->addItems({"Rainbow", "Inverse rainbow", "Custom"});
    ui->colors_frame->hide();

    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->colors->setLayout(new QHBoxLayout());
    ui->colors->layout()->setSizeConstraint(QLayout::SetFixedSize);
    ui->scrollArea->setWidgetResizable(true);

    Defaults();
    ResetNoise();
    ResetColors();
}

NoiseMap::~NoiseMap()
{
    delete noise;
    delete ui;
}

void NoiseMap::ResetColors()
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

    GenerateGradient();
}

void NoiseMap::GenerateGradient()
{
    QGradientStops stops;

    unsigned int colors_count = colors.size();

    float color_step = 1.f / colors_count;

    for(unsigned int i = 0; i < colors_count; i++)
    {
        QGradientStop stop = QGradientStop(
                    i * color_step,
                    QColor(RGBGetRValue(colors[i]), RGBGetGValue(colors[i]), RGBGetBValue(colors[i]))
        );

        stops << stop;
    }

    QGradient::Spread spread = QGradient::PadSpread;

    QLinearGradient grad(QPointF(0,0), QPointF(100,0));
    grad.setSpread(spread);
    grad.setStops(stops);

    QBrush brush = QBrush(grad);
    QRectF rect(0, 0, 100, 1);
    QPainter painter(&image);
    painter.fillRect(rect, brush);
}

ColorPicker* NoiseMap::CreatePicker(int i)
{
    ColorPicker* picker = new ColorPicker();
    picker->SetRGBColor(colors[i]);

    color_pickers[i] = picker;

    connect(picker, &ColorPicker::ColorSelected, [=](QColor c){
        colors[i] = ColorUtils::fromQColor(c);
        GenerateGradient();
    });

    return picker;
}

void NoiseMap::Defaults()
{
    ui->amplitude->setValue(default_amplitude * val_mult);
    ui->frequency->setValue(default_frequency * val_mult);
    ui->lacunarity->setValue(default_lacunarity * val_mult);
    ui->persistence->setValue(default_persistence * val_mult);
    ui->octaves->setValue(default_octaves);
}

void NoiseMap::ResetNoise()
{
    noise = new SimplexNoise(frequency, amplitude, lacunarity, persistence);
}

void NoiseMap::DefineExtraOptions(QLayout* layout)
{
    layout->addWidget(this);
}

void NoiseMap::StepEffect(std::vector<ControllerZone> controller_zones)
{
    for(ControllerZone& controller_zone: controller_zones)
    {
        int start_idx = controller_zone.start_idx();
        zone_type ZT = controller_zone.type();
        int leds_count = controller_zone.leds_count();

        if (ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            for (int LedID = 0; LedID < leds_count; LedID++)
            {
                RGBColor color = GetColor(LedID, 0);
                controller_zone.controller->SetLED(start_idx + LedID, color);
            }
        }

        else if (ZT == ZONE_TYPE_MATRIX)
        {
            int cols = controller_zone.matrix_map_width();
            int rows = controller_zone.matrix_map_height();

            for (int col_id = 0; col_id < cols; col_id++)
            {
                for (int row_id = 0; row_id < rows; row_id++)
                {
                    RGBColor color = GetColor(col_id, row_id);
                    int LedID = controller_zone.controller->zones[controller_zone.zone_idx].matrix_map->map[((row_id * cols) + col_id)];
                    controller_zone.controller->SetLED(start_idx + LedID, color);
                }
            }
        }
    }

    progress +=  0.1 * (float) Speed / (float) FPS;
}

RGBColor NoiseMap::GetColor(unsigned int x, unsigned int y)
{
    float value = noise->fractal(octaves, x, y, progress);
    float frac = (1+value)*0.5;

    int color_choice = ui->colors_choice->currentIndex();
    hsv_t hsv;

    switch (color_choice)
    {
        case 0:
            hsv.hue = 360 * frac;
            hsv.saturation = 255;
            hsv.value = 255 ;
            return RGBColor(hsv2rgb(&hsv));

        case 1:
            hsv.hue = 360 - 360 * frac;
            hsv.saturation = 255;
            hsv.value = 255 ;
            return RGBColor(hsv2rgb(&hsv));

        case 2:
            int color_x = (1 - frac) * 100;
            QColor c = image.pixelColor(color_x, 0);
            return ColorUtils::fromQColor(c);
    }

    return ColorUtils::OFF();
}

void NoiseMap::LoadCustomSettings(json settings)
{
    if(settings.contains("frequency"))  frequency  = settings["frequency"];
    if(settings.contains("amplitude"))  amplitude  = settings["amplitude"];
    if(settings.contains("lacunarity"))  lacunarity  = settings["lacunarity"];
    if(settings.contains("persistence"))  persistence  = settings["persistence"];
    if(settings.contains("octaves"))  octaves  = settings["octaves"];

    ui->amplitude->setValue(amplitude * val_mult);
    ui->frequency->setValue(frequency * val_mult);
    ui->lacunarity->setValue(lacunarity * val_mult);
    ui->persistence->setValue(persistence * val_mult);
    ui->octaves->setValue(octaves);


    if (settings.contains("colors_choice"))
    {
        int colors_choice = settings["colors_choice"];
        ui->colors_choice->setCurrentIndex(colors_choice);
    }

    if (settings.contains("colors"))
    {
        colors.clear();

        for(unsigned int color : settings["colors"])
        {
            colors.push_back(color);
        }

        ui->colors_count_spinBox->setValue(colors.size());

    }
}

json NoiseMap::SaveCustomSettings(json settings)
{
    settings["frequency"] = frequency;
    settings["amplitude"] = amplitude;
    settings["lacunarity"] = lacunarity;
    settings["persistence"] = persistence;
    settings["octaves"] = octaves;

    settings["colors"] = colors;
    settings["colors_choice"] = ui->colors_choice->currentIndex();

    return settings;
}

void NoiseMap::on_amplitude_valueChanged(int value)
{
    amplitude = value * inv_val_mult;
    ResetNoise();
}

void NoiseMap::on_frequency_valueChanged(int value)
{
    frequency = value * inv_val_mult;
    ResetNoise();
}

void NoiseMap::on_lacunarity_valueChanged(int value)
{
    lacunarity = value * inv_val_mult;
    ResetNoise();
}

void NoiseMap::on_persistence_valueChanged(int value)
{
    persistence = value * inv_val_mult;
    ResetNoise();
}

void NoiseMap::on_octaves_valueChanged(int value)
{
    octaves = value;
    ResetNoise();
}

void NoiseMap::on_defaults_clicked()
{
    Defaults();
}

void NoiseMap::on_colors_choice_currentIndexChanged(int value)
{
    ui->colors_frame->setVisible(value==2);
}

void NoiseMap::on_colors_count_spinBox_valueChanged(int)
{
    ResetColors();
}
