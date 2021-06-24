#include "ColorPicker.h"
#include "OpenRGBEffectsPlugin.h"
#include "ui_ColorPicker.h"
#include "ColorWheel.h"

#include <QString>
#include <QFile>
#include <QDialog>
#include <QVBoxLayout>
#include "ColorUtils.h"

ColorPicker::ColorPicker(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColorPicker)
{
    ui->setupUi(this);
    ui->button->setStyleSheet("QPushButton {background-color: #ffffff; border: 1px solid black;}");
}

ColorPicker::~ColorPicker()
{
    delete ui;
}

void ColorPicker::SetQColor(QColor color)
{
    ui->button->setStyleSheet("QPushButton {background-color: "+ color.name() + "; border: 1px solid black;}");
}

void ColorPicker::SetRGBColor(RGBColor color)
{
    SetQColor(ColorUtils::toQColor(color));
}

void ColorPicker::on_button_clicked()
{    
    QPoint button_pos = ui->button->cursor().pos();

    QDialog* dialog = new QDialog();

    if (OpenRGBEffectsPlugin::DarkTheme)
    {
        QPalette pal;
        pal.setColor(QPalette::WindowText, Qt::white);
        dialog->setPalette(pal);
        QFile dark_theme(":/windows_dark.qss");
        dark_theme.open(QFile::ReadOnly);
        dialog->setStyleSheet(dark_theme.readAll());
        dark_theme.close();
    }

    dialog->setMinimumSize(300,320);
    dialog->setModal(true);

    QVBoxLayout* dialog_layout = new QVBoxLayout(dialog);

    ColorWheel* color_wheel = new ColorWheel(dialog);

    dialog_layout->addWidget(color_wheel);

    QHBoxLayout* buttons_layout = new QHBoxLayout();

    QPushButton* accept_button = new QPushButton();
    accept_button->setText("Set Color");
    dialog->connect(accept_button,SIGNAL(clicked()),dialog,SLOT(accept()));
    buttons_layout->addWidget(accept_button);

    QPushButton* cancel_button = new QPushButton();
    cancel_button->setText("Cancel");
    dialog->connect(cancel_button,SIGNAL(clicked()),dialog,SLOT(reject()));
    buttons_layout->addWidget(cancel_button);

    dialog_layout->addLayout(buttons_layout);

    dialog->move(button_pos.x(), button_pos.y());

    if (dialog->exec())
    {
        QColor color = color_wheel->color();
        SetQColor(color);

        emit ColorSelected(color);
        delete dialog;
    }
}
