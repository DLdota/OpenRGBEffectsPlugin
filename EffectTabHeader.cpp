#include "EffectTabHeader.h"
#include "ui_EffectTabHeader.h"
#include <QPropertyAnimation>
#include <QInputDialog>
#include "OpenRGBPluginsFont.h"

EffectTabHeader::EffectTabHeader(QWidget *parent,  RGBEffect* effect) :
    QWidget(parent),
    ui(new Ui::EffectTabHeader)
{
    ui->setupUi(this);

    ui->start_stop->setFont(OpenRGBPluginsFont::GetFont());
    ui->rename->setFont(OpenRGBPluginsFont::GetFont());
    ui->close->setFont(OpenRGBPluginsFont::GetFont());

    ui->rename->setText(OpenRGBPluginsFont::icon(OpenRGBPluginsFont::rename));
    ui->close->setText(OpenRGBPluginsFont::icon(OpenRGBPluginsFont::close));

    ui->effect_name->setText(QString::fromStdString(effect->EffectDetails.EffectName));
    ToogleRunningIndicator(false);

    if(!effect->EffectDetails.CustomName.empty())
    {
        ui->effect_name->setText(QString::fromStdString(effect->EffectDetails.CustomName));
        ui->effect_name->setToolTip(QString::fromStdString(effect->EffectDetails.CustomName));
    }
}

void EffectTabHeader::ToogleRunningIndicator(bool state)
{
    ui->start_stop->setText(
                state ?
                    OpenRGBPluginsFont::icon(OpenRGBPluginsFont::toggle_off):
                    OpenRGBPluginsFont::icon(OpenRGBPluginsFont::toggle_on)
                    );

    ui->start_stop->setToolTip(state?"Started":"Stopped");
}

EffectTabHeader::~EffectTabHeader()
{
    delete ui;
}

void EffectTabHeader::on_close_clicked()
{
    emit CloseRequest();
}

void EffectTabHeader::on_start_stop_clicked()
{
    emit StartStopRequest();
}

void EffectTabHeader::on_rename_clicked()
{
    bool ok;

    QString text = QInputDialog::getMultiLineText(this, "Rename effect", "New name:", ui->effect_name->text(), &ok);

    if (ok && !text.isEmpty())
    {
        ui->effect_name->setText(text);
        ui->effect_name->setToolTip(text);

        emit Renamed(text.toStdString());
    }
}
