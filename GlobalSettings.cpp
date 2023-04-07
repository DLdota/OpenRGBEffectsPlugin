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
    ui->randomColorsCheckBox->setChecked(OpenRGBEffectSettings::globalSettings.prefer_random);
    ui->usePreferedColorsCheckBox->setChecked(OpenRGBEffectSettings::globalSettings.use_prefered_colors);

    ui->preferedColors->SetText("Prefered colors");
    ui->preferedColors->setEnabled(OpenRGBEffectSettings::globalSettings.use_prefered_colors);
    ui->preferedColors->SetColors(OpenRGBEffectSettings::globalSettings.prefered_colors);
}

GlobalSettings::~GlobalSettings()
{
    delete ui;
    OpenRGBEffectSettings::WriteGlobalSettings();
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

void GlobalSettings::on_hide_unsupportedCheckBox_stateChanged(int state)
{
    OpenRGBEffectSettings::globalSettings.hide_unsupported = state;
}

void GlobalSettings::on_usePreferedColorsCheckBox_stateChanged(int state)
{
    ui->preferedColors->setEnabled(state);
    OpenRGBEffectSettings::globalSettings.use_prefered_colors = state;
}

void GlobalSettings::on_preferedColors_ColorsChanged()
{
    OpenRGBEffectSettings::globalSettings.prefered_colors = ui->preferedColors->Colors();
}

void GlobalSettings::on_randomColorsCheckBox_stateChanged(int state)
{
    OpenRGBEffectSettings::globalSettings.prefer_random = state;
}

