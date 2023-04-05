#include "GlobalSettings.h"
#include "OpenRGBEffectsPlugin.h"
#include "ScreenRecorder.h"
#include "json.hpp"

#include <QDesktopServices>
#include <QUrl>
#include <string>

GlobalSettings::GlobalSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GlobalSettings)
{
    ui->setupUi(this);

    int fpscapture = ScreenRecorder::Get()->GetFpsCapture();
    ui->fpscapture->setValue(fpscapture);
}

GlobalSettings::~GlobalSettings()
{
    delete ui;
}

void GlobalSettings::on_fpscapture_valueChanged(int value)
{
    ScreenRecorder::Get()->SetFpsCapture(value);
    ui->fpscapture->setValue(value);
}
