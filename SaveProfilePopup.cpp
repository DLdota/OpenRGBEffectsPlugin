#include "SaveProfilePopup.h"
#include "ui_SaveProfilePopup.h"

#include <QRegExp>
#include <QRegExpValidator>

SaveProfilePopup::SaveProfilePopup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SaveProfilePopup)
{
    ui->setupUi(this);

    // only letters or numbers, - _ and .
    QRegExp re("^[\\w\\-_.]+$");
    QRegExpValidator *validator = new QRegExpValidator(re, this);
    ui->filename->setValidator(validator);
}

SaveProfilePopup::~SaveProfilePopup()
{
    delete ui;
}

QString SaveProfilePopup::Filename()
{
    return ui->filename->text().trimmed();
}

bool SaveProfilePopup::ShouldLoadAtStartup()
{
    return ui->load_at_startup->isChecked();
}

bool SaveProfilePopup::ShouldSaveEffectsState()
{
    return ui->save_effects_state->isChecked();
}

void SaveProfilePopup::SetFileName(QString text)
{
    ui->filename->setText(text);
}

void SaveProfilePopup::on_save_clicked()
{
    emit Accept();
}

void SaveProfilePopup::on_cancel_clicked()
{
    emit Reject();
}
