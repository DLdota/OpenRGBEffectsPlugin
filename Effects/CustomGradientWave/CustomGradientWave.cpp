#include "CustomGradientWave.h"
#include "ColorUtils.h"
#include <QPainter>

REGISTER_EFFECT(CustomGradientWave);

CustomGradientWave::CustomGradientWave(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::CustomGradientWave)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "CustomGradientWave";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Custom gradient wave";
    EffectDetails.IsReversable = true;
    EffectDetails.MaxSpeed     = 200;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.HasCustomSettings = true;

    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->colors->setLayout(new QHBoxLayout());
    ui->colors->layout()->setSizeConstraint(QLayout::SetFixedSize);
    ui->scrollArea->setWidgetResizable(true);

    gradient = QImage(100, 1, QImage::Format_RGB32);

    SetSpeed(25);

    std::map<std::string, std::vector<RGBColor>>::iterator it;

    ui->preset->blockSignals(true);

    for (it = presets.begin(); it != presets.end(); it++)
    {
        ui->preset->addItem(QString::fromStdString(it->first));
    }

    ui->preset->blockSignals(false);

    LoadPreset("Default");
}

void CustomGradientWave::GenerateGradient()
{
    QPointF start_point(0,0);
    QPointF end_point(100,0);

    QLinearGradient grad(start_point, end_point);

    grad.setSpread(QGradient::Spread::PadSpread);

    float step = 1.f / (colors.size() -1);
    float start = 0.f;

    QGradientStops stops;

    for(RGBColor& color: colors)
    {
        QGradientStop stop(start, ColorUtils::toQColor(color));
        start += step;
        stops.append(stop);
    }

    grad.setStops(stops);


    QBrush brush(grad);
    QRectF rect(0, 0, 100, 1);
    QPainter painter(&gradient);

    painter.fillRect(rect, brush);

    ui->gradient->setPixmap(QPixmap::fromImage(gradient));
}

CustomGradientWave::~CustomGradientWave()
{
    delete ui;
}

void CustomGradientWave::DefineExtraOptions(QLayout* layout)
{
    layout->addWidget(this);
}

void CustomGradientWave::StepEffect(std::vector<ControllerZone*> controller_zones)
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
                RGBColor color = GetColor(reverse ? leds_count - LedID - 1 : LedID, leds_count);
                controller_zones[i]->SetLED(start_idx + LedID, color, Brightness);
            }
        }

        else if (ZT == ZONE_TYPE_MATRIX)
        {
            int cols = controller_zones[i]->matrix_map_width();
            int rows = controller_zones[i]->matrix_map_height();

            for (int col_id = 0; col_id < cols; col_id++)
            {
                RGBColor color = GetColor(reverse ? cols - col_id - 1: col_id, cols);

                for (int row_id = 0; row_id < rows; row_id++)
                {
                    int LedID = controller_zones[i]->controller->zones[controller_zones[i]->zone_idx].matrix_map->map[((row_id * cols) + col_id)];
                    controller_zones[i]->SetLED(start_idx + LedID, color, Brightness);
                }
            }

        }
    }

    progress += 0.01 * Speed / (float) FPS;
}

RGBColor CustomGradientWave::GetColor(float x, float w)
{
    float i = x / w + progress;
    i -= (long) i;

    return ColorUtils::fromQColor(gradient.pixelColor(100.0 * i, 0));
}

ColorPicker* CustomGradientWave::CreatePicker(int i)
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

void CustomGradientWave::ResetColors()
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

void CustomGradientWave::on_colors_count_spinBox_valueChanged(int)
{
    ResetColors();
}

void CustomGradientWave::on_preset_currentTextChanged(const QString& text)
{
    LoadPreset(text);
}

void CustomGradientWave::LoadPreset(const QString& text)
{
    colors = presets[text.toStdString()];

    ui->colors_count_spinBox->blockSignals(true);
    ui->colors_count_spinBox->setValue(colors.size());
    ui->colors_count_spinBox->blockSignals(false);

    ResetColors();
}

void CustomGradientWave::LoadCustomSettings(json settings)
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

json CustomGradientWave::SaveCustomSettings(json settings)
{
    settings["colors"] = colors;
    return settings;
}
