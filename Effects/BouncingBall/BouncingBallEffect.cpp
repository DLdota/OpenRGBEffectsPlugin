#include "BouncingBallEffect.h"

REGISTER_EFFECT(BouncingBallEffect);

BouncingBallEffect::BouncingBallEffect(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::BouncingBallEffect)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "Bouncing Ball";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "A ball bounces around your RGB setup";
    EffectDetails.HasCustomSettings = true;

    SetDefaults();
}

BouncingBallEffect::~BouncingBallEffect()
{
    delete ui;

    for (auto s : bouncingBallSimulations)
    {
        delete s;
    }

    bouncingBallSimulations.clear();
}

void BouncingBallEffect::SetDefaults()
{
    ui->ball_radius_slider->setValue(15);
    ui->drop_height_percent_slider->setValue(90);
    ui->gravity_slider->setValue(10);
    ui->horizontal_velocity_slider->setValue(10);
    ui->spectrum_velocity_slider->setValue(10);
}

void BouncingBallEffect::StepEffect(std::vector<ControllerZone*> controllerZones)
{
    for (unsigned int i = 0; i < controllerZones.size(); i++)
    {
        bouncingBallSimulations[i]->DetectSizesChanges();
        bouncingBallSimulations[i]->StepEffect();
    }
}

// Override, in order to re-calculate time delta
void BouncingBallEffect::SetFPS(unsigned int value)
{
    FPS = value;

    for (BouncingBallSimulation* b: bouncingBallSimulations)
    {
        b->SetFps(value);
    }
}

void BouncingBallEffect::SetTemperature(int value)
{
    Temperature = value;

    for (BouncingBallSimulation* b: bouncingBallSimulations)
    {
        b->SetTemperature(value);
    }
}

void BouncingBallEffect::SetTint(int value)
{
    Tint = value;

    for (BouncingBallSimulation* b: bouncingBallSimulations)
    {
        b->SetTint(value);
    }
}

void BouncingBallEffect::SetBrightness(unsigned int value)
{
    Brightness = value;

    for (BouncingBallSimulation* b: bouncingBallSimulations)
    {
        b->SetBrightness(value);
    }
}

void BouncingBallEffect::OnControllerZonesListChanged(std::vector<ControllerZone*> controller_zones)
{
    for (auto s : bouncingBallSimulations)
    {
        delete s;
    }

    bouncingBallSimulations.clear();

    for (unsigned int i = 0; i < controller_zones.size(); i++)
    {
        bouncingBallSimulations.push_back(
            new BouncingBallSimulation(
                controller_zones[i],
                FPS,
                radius,
                gravity,
                horizontalVelocity,
                spectrumVelocity,
                dropHeightPercent,
                Brightness,
                Temperature,
                Tint
            )
        );
    }
}

void BouncingBallEffect::LoadCustomSettings(json Settings)
{
    if(Settings.contains("radius"))                 ui->ball_radius_slider->setValue( Settings["radius"]);
    if(Settings.contains("gravity"))                ui->gravity_slider->setValue(Settings["gravity"]);
    if(Settings.contains("horizontalVelocity"))     ui->horizontal_velocity_slider->setValue(Settings["horizontalVelocity"]);
    if(Settings.contains("spectrumVelocity"))       ui->spectrum_velocity_slider->setValue(Settings["spectrumVelocity"]);
    if(Settings.contains("dropHeightPercent"))      ui->drop_height_percent_slider->setValue(Settings["dropHeightPercent"]);
}

json BouncingBallEffect::SaveCustomSettings()
{
    json settings;

    settings["radius"] = radius;
    settings["gravity"] = gravity;
    settings["horizontalVelocity"] = horizontalVelocity;
    settings["spectrumVelocity"] = spectrumVelocity;
    settings["dropHeightPercent"] = dropHeightPercent;

    return settings;
}

void BouncingBallEffect::on_ball_radius_slider_valueChanged(int value)
{
    radius = value;
    ui->ball_radius_value_label->setText(QString::number(value));

    for (BouncingBallSimulation* b: bouncingBallSimulations)
    {
        b->SetRadius(value);
    }
}

void BouncingBallEffect::on_gravity_slider_valueChanged(int value)
{
    // Update label with gravity value simulation will use
    gravity = BouncingBallSimulation::GetGravity(value);
    ui->gravity_value_label->setText(QString::number(gravity, 'f', 2));

    for (BouncingBallSimulation* b: bouncingBallSimulations)
    {
        b->SetGravity(value);
    }
}

void BouncingBallEffect::on_horizontal_velocity_slider_valueChanged(int value)
{
    horizontalVelocity = value;
    ui->horizontal_velocity_value_label->setText(QString::number(value));

    for (BouncingBallSimulation* b: bouncingBallSimulations)
    {
        b->SetHorizontalVelocity(value);
    }
}

void BouncingBallEffect::on_spectrum_velocity_slider_valueChanged(int value)
{
    spectrumVelocity = value;
    ui->spectrum_velocity_value_label->setText(QString::number(value));

    for (BouncingBallSimulation* b: bouncingBallSimulations)
    {
        b->SetSpectrumVelocity(value);
    }
}

void BouncingBallEffect::on_drop_height_percent_slider_valueChanged(int value)
{
    dropHeightPercent = value;
    ui->drop_height_percent_value_label->setText(QString::number(value));

    for (BouncingBallSimulation* b: bouncingBallSimulations)
    {
        b->SetDropHeightPercent(value);
    }
}

void BouncingBallEffect::on_reset_defaults_button_clicked()
{
    SetDefaults();
}
