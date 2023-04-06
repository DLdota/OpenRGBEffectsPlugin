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

    ui->fpscaptureSlider->setValue(OpenRGBEffectSettings::globalSettings.fpscapture);
    ui->brightnessSlider->setValue(OpenRGBEffectSettings::globalSettings.brightness);
    ui->fpsSlider->setValue(OpenRGBEffectSettings::globalSettings.fps);
    ui->hide_unsupportedCheckBox->setChecked(OpenRGBEffectSettings::globalSettings.hide_unsupported);
}

GlobalSettings::~GlobalSettings()
{
    delete ui;
}

void GlobalSettings::on_fpscaptureSlider_valueChanged(int value)
{
    OpenRGBEffectSettings::globalSettings.fpscapture = value;
}

void GlobalSettings::on_brightnessSlider_valueChanged(int value)
{
    OpenRGBEffectSettings::globalSettings.brightness = value;
}

void GlobalSettings::on_fpsSlider_valueChanged(int value)
{
    OpenRGBEffectSettings::globalSettings.fps = value;
}

void GlobalSettings::on_hide_unsupportedCheckBox_clicked(bool state)
{
    OpenRGBEffectSettings::globalSettings.hide_unsupported = state;
}

void GlobalSettings::on_applyButton_clicked()
{
    OpenRGBEffectSettings::WriteGlobalSettings();
}
