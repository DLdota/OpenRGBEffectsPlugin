#include "ZoneListItem.h"
#include "ui_ZoneListItem.h"

ZoneListItem::ZoneListItem(QString name) :
    QWidget(nullptr),
    ui(new Ui::ZoneListItem)
{
    ui->setupUi(this);

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
    ui->enable->setCheckState(state ? Qt::Checked : Qt::Unchecked);
}

void ZoneListItem::SetReverseChecked(bool state)
{
    ui->reverse->setCheckState(state ? Qt::Checked : Qt::Unchecked);
}

void ZoneListItem::on_enable_clicked()
{
    emit Enabled(ui->enable->isChecked());
}

void ZoneListItem::on_reverse_clicked()
{
    emit Reversed(ui->reverse->isChecked());
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

