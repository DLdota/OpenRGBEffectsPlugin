#include "ColorPicker.h"
#include "OpenRGBEffectsPlugin.h"
#include "ui_ColorPicker.h"
#include "ColorWheel.h"

#include <QString>
#include <QFile>
#include <QDialog>
#include <QVBoxLayout>
#include <QColorDialog>
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
    QColorDialog *colorPicker = new QColorDialog(this);
    colorPicker->setAttribute(Qt::WA_DeleteOnClose);

    connect(colorPicker, &QColorDialog::colorSelected,[=](const QColor &color){
               SetQColor(color);
               emit ColorSelected(color);
    });

    colorPicker->open();
}
