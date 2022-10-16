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
}

void EffectTabHeader::ToogleRunningIndicator(bool state)
{
   //ui->running->setVisible(state);
   ui->running->setText(state?"◉":"○");
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
    QString text = QInputDialog::getText(this, "Rename effect",
                                         "New name:", QLineEdit::Normal,
                                         ui->effect_name->text(), &ok);
    if (ok && !text.isEmpty())
        ui->effect_name->setText(text);
}
