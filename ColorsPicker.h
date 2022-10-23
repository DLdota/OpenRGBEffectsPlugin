#ifndef COLORSPICKER_H
#define COLORSPICKER_H

#include <QWidget>
#include "ColorPicker.h"

namespace Ui {
class ColorsPicker;
}

class ColorsPicker : public QWidget
{
    Q_OBJECT

public:
    explicit ColorsPicker(QWidget *parent = nullptr);
    ~ColorsPicker();

    std::vector<RGBColor> Colors();
    void SetColors(std::vector<RGBColor>);

signals:
    void ColorsChanged();

private slots:
    void on_colors_count_spinBox_valueChanged(int);

private:
    Ui::ColorsPicker *ui;

    void ResetColors();
    ColorPicker* CreatePicker(int);
    std::vector<RGBColor> colors;
    std::vector<ColorPicker*> color_pickers;

};

#endif // COLORSPICKER_H
