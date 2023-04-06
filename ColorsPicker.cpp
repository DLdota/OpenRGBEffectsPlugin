#include "ColorsPicker.h"
#include "ui_ColorsPicker.h"
#include "ColorUtils.h"

ColorsPicker::ColorsPicker(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColorsPicker)
{
    ui->setupUi(this);

    ui->colors->setLayout(new QHBoxLayout());
    ui->colors->layout()->setAlignment(Qt::AlignLeft );

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
        emit ColorsChanged();
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

    emit ColorsChanged();
}
void ColorsPicker::SetColors(std::vector<RGBColor> value)
{
    colors = value;

    ui->colors_count_spinBox->blockSignals(true);
    ui->colors_count_spinBox->setValue(colors.size());
    ui->colors_count_spinBox->blockSignals(false);

    ResetColors();
}

void ColorsPicker::SetText(std::string text)
{
    ui->colors_label->setText(QString::fromStdString(text));
}

void ColorsPicker::on_colors_count_spinBox_valueChanged(int)
{
    ResetColors();
}

std::vector<RGBColor> ColorsPicker::Colors()
{
    return colors;
}
