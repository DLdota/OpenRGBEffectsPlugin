#include "Ambient.h"
#include "ColorUtils.h"
#include "RectangleSelector.h"
#include "ScreenRecorder.h"

REGISTER_EFFECT(Ambient);

Ambient::Ambient(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::Ambient)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "Ambient";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Takes a portion of the screen and reflect it to your devices";
    EffectDetails.HasCustomSettings = true;
    EffectDetails.SupportsRandom = false;

    ui->mode->addItems({
                           "Scaled average",
                           "Screen copy",
                           "Calculated average (heavy computing)",
                           "Most common color (heavy computing)",
                       });

    rectangle_selector_overlay = new RectangleSelectorOverlay(this);

    connect(rectangle_selector_overlay, &RectangleSelectorOverlay::SelectionUpdated, [=](QRect rect){
        ui->left->setValue(rect.left());
        ui->top->setValue(rect.top());
        ui->width->setValue(rect.width());
        ui->height->setValue(rect.height());
    });

    QList<QScreen*> screens = QGuiApplication::screens();

    for(QScreen* screen: screens)
    {
        ui->screen->addItem(screen->name());
    }

    ScreenRecorder::Get()->SetScreen(0);
    ScreenRecorder::Get()->SetRect(QRect(left, top, width, height));
}

Ambient::~Ambient()
{
    delete rectangle_selector_overlay;
    delete ui;
}

void Ambient::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    if(controller_zones.empty())
    {
        return;
    }

    if(width == 0 || height == 0)
    {
        for(ControllerZone* controller_zone : controller_zones)
        {
            controller_zone->SetAllZoneLEDs(0, Brightness, Temperature, Tint);
        }

        return;
    }

    const QImage& image = ScreenRecorder::Get()->Capture();

    int w = image.width();
    int h = image.height();

    switch (mode) {

    case CALCULATED_AVERAGE:
    {
        int r = 0 , g = 0 , b = 0;

        RGBColor color;

        if(w > 0 && h > 0)
        {
            for (int x = 0; x < w; x++)
            {
                for (int y = 0; y < h; y++)
                {
                    QColor c = image.pixelColor(x,y);
                    r += c.red();
                    g += c.green();
                    b += c.blue();
                }
            }

            int cr = (r / (w * h));
            int cg = (g / (w * h));
            int cb = (b / (w * h));

            color = ToRGBColor(cr, cg, cb);
        }
        else
        {
            color = ColorUtils::OFF();
        }

        for(ControllerZone* controller_zone : controller_zones)
        {
            controller_zone->SetAllZoneLEDs(Smooth(color), Brightness, Temperature, Tint);
        }

        break;
    }

    case MOST_COMMON:
    {
        std::vector<RGBColor> image_colors;

        std::unordered_map<RGBColor, int> colors;
        RGBColor most_common = ColorUtils::OFF();
        int max_count = 0;

        for (int x = 0; x < w; x++)
        {
            for (int y = 0; y < h; y++)
            {
                RGBColor c = ColorUtils::fromQColor(image.pixelColor(x,y));

                if (++colors[c] > max_count) {
                    most_common = c;
                    max_count = colors[c];
                }
            }
        }

        RGBColor smoothed = Smooth(most_common);

        for(ControllerZone* controller_zone : controller_zones)
        {
            controller_zone->SetAllZoneLEDs(smoothed, Brightness, Temperature, Tint);
        }

        break;
    }

    case SCALED_AVERAGE:
    {
        QImage scaled_internal = image.scaled(1, 1);

        RGBColor color = ColorUtils::fromQColor(scaled_internal.pixelColor(0,0));

        RGBColor smoothed = Smooth(color);
        for(ControllerZone* controller_zone : controller_zones)
        {
            controller_zone->SetAllZoneLEDs(smoothed, Brightness, Temperature, Tint);
        }

        break;
    }

    case SCREEN_COPY:
    {
        for(ControllerZone* controller_zone : controller_zones)
        {
            unsigned int leds_count = controller_zone->leds_count();
            bool reverse = controller_zone->reverse;

            if(controller_zone->type() == ZONE_TYPE_SINGLE || controller_zone->type() == ZONE_TYPE_LINEAR)
            {
                unsigned int width = controller_zone->leds_count();
                unsigned int height = 1;

                QImage scaled = image.scaled(width, height);

                for(unsigned int i = 0; i < width; i++)
                {                   
                    QColor color = scaled.pixelColor(reverse ? leds_count - i - 1 : i, 0);
                    controller_zone->SetLED(i, SmoothMatrix(ColorUtils::fromQColor(color), i, 0), Brightness, Temperature, Tint);
                }

            }
            else if(controller_zone->type() == ZONE_TYPE_MATRIX)
            {
                unsigned int width = controller_zone->matrix_map_width();
                unsigned int height = controller_zone->matrix_map_height();
                unsigned int * map = controller_zone->map();

                QImage scaled = image.scaled(width, height);

                for(unsigned int h = 0; h < height; h++)
                {
                    for(unsigned int w = 0; w <  width; w++)
                    {
                        QColor color = scaled.pixelColor(reverse ? width - w - 1: w, h);
                        unsigned int led_num = map[h * width + w];
                        controller_zone->SetLED(led_num, SmoothMatrix(ColorUtils::fromQColor(color), w, h), Brightness, Temperature, Tint);
                    }
                }

            }
        }

        break;
    }
    }

}

RGBColor Ambient::Smooth(RGBColor color)
{
    RGBColor smoothed;

    if(color != old_single_color)
    {
        float smoothness_f = smoothness;
        smoothed = ColorUtils::Interpolate(old_single_color, color, smoothness_f / 1000);
        old_single_color = smoothed;
    }
    else {
        smoothed = color;
    }

    return smoothed;
}

RGBColor Ambient::SmoothMatrix(RGBColor color, int w, int h)
{
    if(color != previous[w][h])
    {
        float smoothness_f = smoothness;
        color = ColorUtils::Interpolate(previous[w][h], color, smoothness_f / 1000);
        previous[w][h] = color;
    }

    return color;
}

void Ambient::EffectState(bool state)
{
    if(state)
    {
        ScreenRecorder::Get()->Start();
    }
    else
    {
        ScreenRecorder::Get()->Stop();
    }
}

void Ambient::LoadCustomSettings(json settings)
{
    if(settings.contains("left"))
    {
        ui->left->setValue(settings["left"]);
    }

    if(settings.contains("top"))
    {
        ui->top->setValue(settings["top"]);
    }

    if(settings.contains("width"))
    {
        ui->width->setValue(settings["width"]);
    }

    if(settings.contains("height"))
    {
        ui->height->setValue(settings["height"]);
    }

    if(settings.contains("mode"))
    {
        ui->mode->setCurrentIndex(settings["mode"]);
    }

    if(settings.contains("screen_index"))
    {
        ui->screen->setCurrentIndex(settings["screen_index"]);
    }
    if(settings.contains("smoothness"))
    {
        ui->smoothness->setValue(settings["smoothness"]);
    }
}

json Ambient::SaveCustomSettings()
{
    json settings;

    settings["left"] = left;
    settings["top"] = top;
    settings["height"] = height;
    settings["width"] = width;
    settings["mode"] = mode;
    settings["screen_index"] = screen_index;
    settings["smoothness"] = smoothness;
    return settings;
}

void Ambient::on_left_valueChanged(int value)
{
    left = value;
    ScreenRecorder::Get()->SetRect(QRect(left, top, width, height));
}

void Ambient::on_top_valueChanged(int value)
{
    top = value;
    ScreenRecorder::Get()->SetRect(QRect(left, top, width, height));
}

void Ambient::on_width_valueChanged(int value)
{
    width = value;
    ScreenRecorder::Get()->SetRect(QRect(left, top, width, height));
}

void Ambient::on_height_valueChanged(int value)
{
    height = value;
    ScreenRecorder::Get()->SetRect(QRect(left, top, width, height));
}

void Ambient::on_mode_currentIndexChanged(int value)
{
    mode = static_cast<AmbientMode>(value);
}

void Ambient::on_screen_currentIndexChanged(int value)
{
    screen_index = value;
    ScreenRecorder::Get()->SetScreen(screen_index);
}


void Ambient::on_select_rectangle_clicked() {
    rectangle_selector_overlay->StartSelection(screen_index);
}

void Ambient::on_smoothness_valueChanged(int value)
{
    if (value > 1000) {
        value = 1000;
    }
    smoothness = 1000 - value + 1;
}
