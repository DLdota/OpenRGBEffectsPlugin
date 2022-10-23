#include "ColorsPicker.h"
#include "ui_ColorsPicker.h"
#include "ColorUtils.h"

ColorsPicker::ColorsPicker(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColorsPicker)
{
    ui->setupUi(this);

    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->colors->setLayout(new QHBoxLayout());
    ui->colors->layout()->setSizeConstraint(QLayout::SetFixedSize);
    ui->scrollArea->setWidgetResizable(true);

    ResetColors();
}

ColorsPicker::~ColorsPicker()
{
    delete ui;
}

ColorPicker* ColorsPicker::CreatePicker(int i)
{
    ColorPicker* picker = new ColorPicker();
    picker->SetRGBColor(colors[i]);

    color_pickers[i] = picker;

    connect(picker, &ColorPicker::ColorSelected, [=](QColor c){
        colors[i] = ColorUtils::fromQColor(c);
    });

    return picker;
}

void ColorsPicker::ResetColors()
{
    QLayoutItem *child;

    while ((child = ui->colors->layout()->takeAt(0)) != 0) {
        delete child->widget();
    }

    unsigned int colors_count = ui->colors_count_spinBox->value();

    color_pickers.resize(colors_count);
    colors.resize(colors_count);

    for(unsigned int i = 0; i < colors_count; i++)
    {
        ColorPicker* picker = CreatePicker(i);
        ui->colors->layout()->addWidget(picker);
    }
}
void ColorsPicker::SetColors(std::vector<RGBColor> value)
{
    colors = value;
    ui->colors_count_spinBox->setValue(colors.size());
}

void ColorsPicker::on_colors_count_spinBox_valueChanged(int)
{
    ResetColors();
}

std::vector<RGBColor> ColorsPicker::Colors()
{
    return colors;
}
