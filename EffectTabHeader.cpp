#include "EffectTabHeader.h"
#include "ui_EffectTabHeader.h"
#include <QPropertyAnimation>

EffectTabHeader::EffectTabHeader(QWidget *parent,  RGBEffect* effect) :
    QWidget(parent),
    ui(new Ui::EffectTabHeader)
{
    ui->setupUi(this);
    ui->effect_name->setText(QString::fromStdString(effect->EffectDetails.EffectName));
    ToogleRunningIndicator(false);
}

void EffectTabHeader::ToogleRunningIndicator(bool state)
{
   ui->running->setVisible(state);
}

EffectTabHeader::~EffectTabHeader()
{
    delete ui;
}


void EffectTabHeader::on_close_clicked()
{
    emit CloseRequest();
}
