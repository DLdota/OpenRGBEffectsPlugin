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

    screen_recorder.SetScreen(0);
    screen_recorder.SetRect(QRect(left, top, width, height));
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
            controller_zone->SetAllZoneLEDs(0, Brightness);
        }

        return;
    }

    const QImage& image = screen_recorder.Capture();

    switch (mode) {

    case CALCULATED_AVERAGE:
    {
        int r = 0 , g = 0 , b = 0;
        int w = image.width();
        int h = image.height();

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
            controller_zone->SetAllZoneLEDs(color, Brightness);
        }

        break;
    }

    case MOST_COMMON:
    {
        std::vector<RGBColor> image_colors;
        int w = image.width();
        int h = image.height();

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
            controller_zone->SetAllZoneLEDs(smoothed, Brightness);
        }

        break;
    }

    case SCALED_AVERAGE:
    {
        QImage scaled = image.scaled(1, 1);

        RGBColor color = ColorUtils::fromQColor(scaled.pixelColor(0,0));

        RGBColor smoothed = Smooth(color);

        for(ControllerZone* controller_zone : controller_zones)
        {
            controller_zone->SetAllZoneLEDs(smoothed, Brightness);
        }

        break;
    }

    case SCREEN_COPY:
    {
        for(ControllerZone* controller_zone : controller_zones)
        {
            unsigned int start_idx = controller_zone->start_idx();
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
                    controller_zone->SetLED(start_idx + i, ColorUtils::fromQColor(color), Brightness);
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
                        controller_zone->SetLED(start_idx + led_num, ColorUtils::fromQColor(color), Brightness);
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
        smoothed = ColorUtils::Interpolate(old_single_color, color, 0.05f);
        old_single_color = smoothed;
    }
    else {
        smoothed = color;
    }

    return smoothed;
}

void Ambient::EffectState(bool state)
{
    if(state)
    {
        screen_recorder.Start();
    }
    else
    {
        screen_recorder.Stop();
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
    return settings;
}

void Ambient::on_left_valueChanged(int value)
{
    left = value;
    screen_recorder.SetRect(QRect(left, top, width, height));
}

void Ambient::on_top_valueChanged(int value)
{
    top = value;
    screen_recorder.SetRect(QRect(left, top, width, height));
}

void Ambient::on_width_valueChanged(int value)
{
    width = value;
    screen_recorder.SetRect(QRect(left, top, width, height));
}

void Ambient::on_height_valueChanged(int value)
{
    height = value;
    screen_recorder.SetRect(QRect(left, top, width, height));
}

void Ambient::on_mode_currentIndexChanged(int value)
{
    mode = static_cast<AmbientMode>(value);
}

void Ambient::on_screen_currentIndexChanged(int value)
{
    screen_index = value;
    screen_recorder.SetScreen(screen_index);
}


void Ambient::on_select_rectangle_clicked() {
    rectangle_selector_overlay->StartSelection(screen_index);
}
