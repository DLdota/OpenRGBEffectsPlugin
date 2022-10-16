#include "EffectTabHeader.h"
#include "ui_EffectTabHeader.h"
#include <QPropertyAnimation>
#include <QInputDialog>

EffectTabHeader::EffectTabHeader(QWidget *parent,  RGBEffect* effect) :
    QWidget(parent),
    ui(new Ui::EffectTabHeader)
{
    ui->setupUi(this);
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
   ui->running->setText(state?"◉":"○");
   ui->running->setToolTip(state?"Started":"Stopped");
}

EffectTabHeader::~EffectTabHeader()
{
    delete ui;
}

void EffectTabHeader::on_close_clicked()
{
    emit CloseRequest();
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
