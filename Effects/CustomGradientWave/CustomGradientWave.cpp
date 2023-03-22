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
    EffectDetails.EffectDescription = "Create your own gradient wave or use predefined color set";
    EffectDetails.IsReversable = true;
    EffectDetails.MaxSpeed     = 200;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.HasCustomSettings = true;
    EffectDetails.SupportsRandom = false;

    gradient = QImage(100, 1, QImage::Format_RGB32);

    SetSpeed(25);

    for(const CustomGradientWavePreset& preset: presets)
    {
        ui->preset->addItem(QString::fromStdString(preset.name));
    }

    ui->preset->blockSignals(false);
    ui->direction->addItems({"Horizontal", "Vertical", "Radial out", "Radial in"});

    LoadPreset("Default");
}

void CustomGradientWave::GenerateGradient()
{
    QPointF start_point(0,0);
    QPointF end_point(100,0);

    QLinearGradient grad(start_point, end_point);

    grad.setSpread(QGradient::Spread::PadSpread);

    std::vector<RGBColor> colors = ui->colorsPicker->Colors();
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

void CustomGradientWave::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    for(unsigned int i = 0; i < controller_zones.size(); i++)
    {
        zone_type ZT = controller_zones[i]->type();
        int leds_count = controller_zones[i]->leds_count();
        bool reverse = controller_zones[i]->reverse;

        if (ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            for (int LedID = 0; LedID < leds_count; LedID++)
            {
                RGBColor color = GetColor(reverse ? leds_count - LedID - 1 : LedID, 0, leds_count, 1);
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
                    RGBColor color = GetColor(reverse ? cols - col_id - 1: col_id, row_id, cols, rows);

                    int LedID = controller_zones[i]->controller->zones[controller_zones[i]->zone_idx].matrix_map->map[((row_id * cols) + col_id)];
                    controller_zones[i]->SetLED(LedID, color, Brightness);
                }
            }

        }
    }

    progress += 0.01 * Speed / (float) FPS;
}

RGBColor CustomGradientWave::GetColor(float x, float y, float w, float h)
{
    float i = 0;
    float cy = (h-1)*(0.01*height);
    float cx = (w-1)*(0.01*width);
    float distance = 0;

    switch(direction)
    {
        case HORIZONTAL:
            i = (spread/100.f) * x / w + progress;
        break;

        case VERTICAL:
            i = (spread/100.f) * y / h + progress;
        break;

        case RADIAL_IN:
            distance = sqrt(pow(x - cx, 2.0) + pow(y - cy, 2.0));
            i = fabs((spread/100.f) * distance / w + progress);

        break;

        case RADIAL_OUT:
            distance = sqrt(pow(x - cx, 2.0) + pow(y - cy, 2.0));
            i = fabs((spread/100.f) * distance / w - progress);

        break;

        default: break;
    }

    i -= (long) i;

    return ColorUtils::fromQColor(gradient.pixelColor(100.0 * i, 0));
}

void CustomGradientWave::on_colorsPicker_ColorsChanged()
{
    GenerateGradient();
}

void CustomGradientWave::on_spread_valueChanged(int value)
{
    spread = value;
}

void CustomGradientWave::on_preset_currentTextChanged(const QString& text)
{
    LoadPreset(text);
}

void CustomGradientWave::on_direction_currentIndexChanged(int value)
{
    direction = value;
}

void CustomGradientWave::on_height_valueChanged(int value)
{
    height = value;
}

void CustomGradientWave::on_width_valueChanged(int value)
{
    width = value;
}

void CustomGradientWave::LoadPreset(const QString& text)
{
    std::string preset_name = text.toStdString();

    for(const CustomGradientWavePreset& preset: presets)
    {
        if(preset_name == preset.name){
            ui->colorsPicker->SetColors(preset.colors);
            break;
        }
    }
}

void CustomGradientWave::LoadCustomSettings(json settings)
{
    if (settings.contains("colors"))
    {
        ui->colorsPicker->SetColors(settings["colors"]);
    }

    if (settings.contains("spread"))
    {
        ui->spread->setValue(settings["spread"]);
    }

    if (settings.contains("direction"))
    {
        ui->direction->setCurrentIndex(settings["direction"]);
    }

    if (settings.contains("height"))
    {
        ui->height->setValue(settings["height"]);
    }

    if (settings.contains("width"))
    {
        ui->width->setValue(settings["width"]);
    }

}

json CustomGradientWave::SaveCustomSettings()
{
    json settings;

    settings["colors"]    = ui->colorsPicker->Colors();
    settings["spread"]    = spread;
    settings["direction"] = direction;
    settings["height"]    = height;
    settings["width"]     = width;

    return settings;
}
