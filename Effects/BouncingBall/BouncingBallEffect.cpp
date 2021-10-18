#include "BouncingBallEffect.h"

REGISTER_EFFECT(BouncingBallEffect);

BouncingBallEffect::BouncingBallEffect(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::BouncingBall)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "Bouncing Ball";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "A ball bounces around your RGB setup";
    EffectDetails.HasCustomSettings = true;

    radius = ui->ball_radius_slider->value();
    gravity = ui->gravity_slider->value();
    horizontalVelocity = ui->horizontal_velocity_slider->value();
    spectrumVelocity = ui->spectrum_velocity_slider->value();
    dropHeightPercent = ui->drop_height_percent_slider->value();
}

BouncingBallEffect::~BouncingBallEffect()
{
    delete ui;

    for (auto& sim : bouncingBallSimulations) {
        delete(sim.second);
    }
}

void BouncingBallEffect::DefineExtraOptions(QLayout* layout)
{
    layout->addWidget(this);
}

void BouncingBallEffect::StepEffect(std::vector<ControllerZone*> controllerZones)
{
    for (ControllerZone* controllerZone : controllerZones) {
        BouncingBallSimulation* bouncingBallSim;
        auto kvp = bouncingBallSimulations.find(controllerZone);

        if (kvp != bouncingBallSimulations.end()) {
            bouncingBallSim = kvp->second;
            checkForDimensionChange(kvp->first, kvp->second);
        } else {
            bouncingBallSim = new BouncingBallSimulation(
                controllerZone,
                FPS,
                radius,
                gravity,
                horizontalVelocity,
                spectrumVelocity,
                dropHeightPercent);

            std::pair<ControllerZone*, BouncingBallSimulation*> pair(controllerZone, bouncingBallSim);
            bouncingBallSimulations.insert(pair);
        }

        bouncingBallSim->StepEffect();
    }
}

// Override, in order to re-calculate time delta
void BouncingBallEffect::SetFPS(unsigned int value)
{
    FPS = value;

    for (const auto& pair : bouncingBallSimulations) {
        pair.second->SetFps(FPS);
    }
}

void BouncingBallEffect::SetBrightness(unsigned int value)
{
    Brightness = value;

    for (const auto& pair : bouncingBallSimulations) {
        pair.second->SetBrightness(Brightness);
    }
}

void BouncingBallEffect::checkForDimensionChange(
    ControllerZone* controllerZone,
    BouncingBallSimulation* bouncingBallSim)
{
    if (controllerZone->type() == ZONE_TYPE_MATRIX) {
        // Manually check for on-the-fly changes in matrix width or height.
        // Width and height are not updated via slots through the UI.
        int newWidth = controllerZone->matrix_map_width();
        if (bouncingBallSim->GetWidth() != newWidth) {
            bouncingBallSim->SetWidth(newWidth);
        }

        int newHeight = controllerZone->matrix_map_height();
        if (bouncingBallSim->GetHeight() != newHeight) {
            bouncingBallSim->SetHeight(newHeight);
        }
    }
}

void BouncingBallEffect::LoadCustomSettings(json Settings)
{
    if(Settings.contains("radius"))
        radius = Settings["radius"];

    if(Settings.contains("gravity"))
        gravity = Settings["gravity"];

    if(Settings.contains("horizontalVelocity"))
        horizontalVelocity = Settings["horizontalVelocity"];

    if(Settings.contains("spectrumVelocity"))
        spectrumVelocity = Settings["spectrumVelocity"];

    if(Settings.contains("dropHeightPercent"))
        dropHeightPercent = Settings["dropHeightPercent"];

    ui->ball_radius_slider->setValue(radius);
    ui->gravity_slider->setValue(gravity);
    ui->horizontal_velocity_slider->setValue(horizontalVelocity);
    ui->spectrum_velocity_slider->setValue(spectrumVelocity);
    ui->drop_height_percent_slider->setValue(dropHeightPercent);
}

json BouncingBallEffect::SaveCustomSettings(json Settings)
{
    Settings["radius"] = radius;
    Settings["gravity"] = gravity;
    Settings["horizontalVelocity"] = horizontalVelocity;
    Settings["spectrumVelocity"] = spectrumVelocity;
    Settings["dropHeightPercent"] = dropHeightPercent;

    return Settings;
}

void BouncingBallEffect::OnControllerZonesListChanged(std::vector<ControllerZone*> newControllerZones)
{
    std::unordered_set<ControllerZone*> lastKnownControllerZones;

    for (const auto& oldControllerZone : bouncingBallSimulations) {
        lastKnownControllerZones.insert(oldControllerZone.first);
    }

    for (ControllerZone* newControllerZone : newControllerZones) {
        lastKnownControllerZones.erase(newControllerZone);
    }

    for (ControllerZone* removedControllerZone : lastKnownControllerZones) {
        delete bouncingBallSimulations[removedControllerZone];
        bouncingBallSimulations.erase(removedControllerZone);
    }
}


void BouncingBallEffect::on_ball_radius_slider_valueChanged(int value)
{
    radius = value;
    ui->ball_radius_value_label->setText(QString::number(value));

    for (const auto& pair : bouncingBallSimulations) {
        pair.second->SetRadius(radius);
    }
}

void BouncingBallEffect::on_gravity_slider_valueChanged(int value)
{
    // Update label with gravity value simulation will use
    gravity = BouncingBallSimulation::GetGravity(value);
    ui->gravity_value_label->setText(QString::number(gravity, 'f', 2));

    for (const auto& pair : bouncingBallSimulations) {
        // Simulation will recalculate correct gravity value
        pair.second->SetGravity(value);
    }
}

void BouncingBallEffect::on_horizontal_velocity_slider_valueChanged(int value)
{
    horizontalVelocity = value;
    ui->horizontal_velocity_value_label->setText(QString::number(value));

    for (const auto& pair : bouncingBallSimulations) {
        pair.second->SetHorizontalVelocity(horizontalVelocity);
    }
}

void BouncingBallEffect::on_spectrum_velocity_slider_valueChanged(int value)
{
    spectrumVelocity = value;
    ui->spectrum_velocity_value_label->setText(QString::number(value));

    for (const auto& pair : bouncingBallSimulations) {
        pair.second->SetSpectrumVelocity(spectrumVelocity);
    }
}

void BouncingBallEffect::on_drop_height_percent_slider_valueChanged(int value)
{
    dropHeightPercent = value;
    ui->drop_height_percent_value_label->setText(QString::number(value));

    for (const auto& pair : bouncingBallSimulations) {
        pair.second->SetDropHeightPercent(dropHeightPercent);
    }
}

void BouncingBallEffect::on_reset_defaults_button_clicked()
{
    ui->ball_radius_slider->setValue(15);
    ui->drop_height_percent_slider->setValue(90);
    ui->gravity_slider->setValue(10);
    ui->horizontal_velocity_slider->setValue(10);
    ui->spectrum_velocity_slider->setValue(10);
}
