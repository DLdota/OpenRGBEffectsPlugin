#include "ZoneListItem.h"
#include "ui_ZoneListItem.h"

ZoneListItem::ZoneListItem(QString name) :
    QWidget(nullptr),
    ui(new Ui::ZoneListItem)
{
    ui->setupUi(this);
    ui->brightness->setVisible(false);
    ui->zone_name->setText(name);
}

ZoneListItem::~ZoneListItem()
{
    delete ui;
}

void ZoneListItem::DisableControls()
{
    ui->enable->setDisabled(true);
    ui->reverse->setDisabled(true);
}

void ZoneListItem::EnableControls()
{
    ui->enable->setDisabled(false);
    ui->reverse->setDisabled(false);
}

void ZoneListItem::ToggleBrightnessSlider()
{
    ui->brightness->setVisible(!ui->brightness->isVisible());
}

void ZoneListItem::SetEnableChecked(bool state)
{
    ui->enable->blockSignals(true);
    ui->enable->setChecked(state);
    ui->enable->blockSignals(false);
}

void ZoneListItem::SetReverseChecked(bool state)
{
    ui->reverse->blockSignals(true);
    ui->reverse->setChecked(state);
    ui->reverse->blockSignals(false);
}

void ZoneListItem::on_enable_toggled(bool state)
{
    emit Enabled(state);
}

void ZoneListItem::on_reverse_toggled(bool state)
{
    emit Reversed(state);
}

void ZoneListItem::on_brightness_valueChanged(int value)
{
    emit BrightnessChanged(value);
}

bool ZoneListItem::IsEnabled()
{
    return ui->enable->isChecked();
}

bool ZoneListItem::IsReversed()
{
    return ui->reverse->isChecked();
}

void ZoneListItem::SetBrightness(int value)
{
    ui->brightness->setValue(value);
}

