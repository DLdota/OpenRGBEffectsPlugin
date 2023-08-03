#include "RandomSpin.h"
#include <QPainter>

REGISTER_EFFECT(RandomSpin);

RandomSpin::RandomSpin(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::RandomSpin)
{
    ui->setupUi(this);

    EffectDetails.EffectName        = "RandomSpin";
    EffectDetails.EffectClassName   = ClassName();
    EffectDetails.EffectDescription = "A simple RandomSpin for your devices";
    EffectDetails.IsReversable      = true;
    EffectDetails.MaxSpeed          = 100;
    EffectDetails.MinSpeed          = 1;
    EffectDetails.UserColors        = 1;
    EffectDetails.SupportsRandom    = false;

    gradient = QImage(100, 1, QImage::Format_RGB32);

    SetSpeed(50);
}

RandomSpin::~RandomSpin()
{
    delete ui;
}

void RandomSpin::GenerateGradient()
{
    QPointF start_point(0,0);
    QPointF end_point(100,0);

    QLinearGradient grad(start_point, end_point);

    grad.setSpread(QGradient::Spread::PadSpread);

    std::vector<std::tuple<float, RGBColor>> stop_entries = {
        { 0.f      , 0 },
        { 0.15f    , 0 },
        { 0.25f    , UserColors[0] },
        { 0.35f    , 0 },
        { 0.f      , 0 },
        { 0.65f    , 0 },
        { 0.75f    , UserColors[0] },
        { 0.85f    , 0 },
        { 0.8f     , 0 }
    };

    QGradientStops stops;

    for(const std::tuple<float, RGBColor>& stop_entry: stop_entries)
    {
        QGradientStop stop(std::get<0>(stop_entry), ColorUtils::toQColor(std::get<1>(stop_entry)));
        stops.append(stop);
    }

    grad.setStops(stops);

    QBrush brush(grad);
    QRectF rect(0, 0, 100, 1);
    QPainter painter(&gradient);
    painter.fillRect(rect, brush);
}

double RandomSpin::custom_rand(double min, double max)
{
    double d = (double)rand() / RAND_MAX;
    return min + d * (max - min);
}

void RandomSpin::OnControllerZonesListChanged(std::vector<ControllerZone*> controller_zones)
{
    GenerateGradient();
    spin_entries.clear();
    spin_entries.resize(controller_zones.size());
}

void RandomSpin::StepEffect(std::vector<ControllerZone*> controller_zones)
{    
    for(unsigned int i = 0; i < controller_zones.size(); i++)
    {
        int leds_count = controller_zones[i]->leds_count();

        if(leds_count == 0)
        {
            continue;
        }

        bool reverse = controller_zones[i]->reverse;

        if(spin_entries[i].next_time_point < progress)
        {
            spin_entries[i].stop            = !spin_entries[i].stop;

            spin_entries[i].next_time_point = progress + custom_rand(1, spin_entries[i].stop ? 1.5 : 3.5);
            spin_entries[i].speed_mult      = custom_rand(1.f, 5.f);
            spin_entries[i].dir             = rand() % 2 == 0;
        }
        else
        {
            spin_entries[i].progress += (spin_entries[i].dir ? -1: 1) * 0.01 * Speed / (float) FPS;
        }

        if(spin_entries[i].stop)
        {
            continue;
        }

        for (int LedID = 0; LedID < leds_count; LedID++)
        {
            RGBColor color = GetColor(reverse ? leds_count - LedID - 1 : LedID, leds_count, spin_entries[i]);
            controller_zones[i]->SetLED(LedID, color, Brightness, Temperature, Tint);
        }
    }

    progress += 0.01 * Speed / (float) FPS;
}

RGBColor RandomSpin::GetColor(unsigned int i, unsigned int w, const RandomSpinEntry& entry)
{
    double percent = (double)i / (double)w;
    percent += std::fabs(entry.progress * entry.speed_mult);
    percent -= (unsigned long) percent;

    QPoint p(percent * 100, 0);

    QColor c = gradient.pixelColor(p);
    return ColorUtils::fromQColor(c);
}
