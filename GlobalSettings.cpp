#include "GlobalSettings.h"
#include "OpenRGBEffectsPlugin.h"
#include "ScreenRecorder.h"
#include "json.hpp"
#include "OpenRGBEffectSettings.h"

#include <QDesktopServices>
#include <QUrl>
#include <string>

GlobalSettings::GlobalSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GlobalSettings)
{
    ui->setupUi(this);

    ui->fpscaptureSlider->blockSignals(true);
    ui->brightnessSlider->blockSignals(true);
    ui->fpsSlider->blockSignals(true);

    ui->fpscaptureSlider->setValue(OpenRGBEffectSettings::globalSettings.fpscapture);
    ui->brightnessSlider->setValue(OpenRGBEffectSettings::globalSettings.brightness);
    ui->fpsSlider->setValue(OpenRGBEffectSettings::globalSettings.fps);

    ui->fpscaptureSlider->blockSignals(false);
    ui->brightnessSlider->blockSignals(false);
    ui->fpsSlider->blockSignals(false);
}

GlobalSettings::~GlobalSettings()
{
    delete ui;
}

void GlobalSettings::on_fpscaptureSlider_valueChanged(int value)
{
    OpenRGBEffectSettings::globalSettings.fpscapture = value;
    OpenRGBEffectSettings::WriteGlobalSettings();
}

void GlobalSettings::on_brightnessSlider_valueChanged(int value)
{
    OpenRGBEffectSettings::globalSettings.brightness = value;
    OpenRGBEffectSettings::WriteGlobalSettings();
}

void GlobalSettings::on_fpsSlider_valueChanged(int value)
{
    OpenRGBEffectSettings::globalSettings.fps = value;
    OpenRGBEffectSettings::WriteGlobalSettings();
}
