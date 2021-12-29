#ifndef COLORPICKER_H
#define COLORPICKER_H

#include <QWidget>
#include "RGBController.h"

namespace Ui {
class ColorPicker;
}

class ColorPicker : public QWidget
{
    Q_OBJECT

public:
    explicit ColorPicker(QWidget *parent = nullptr);
    ~ColorPicker();

    void SetQColor(QColor);
    void SetRGBColor(RGBColor);

private slots:
    void on_button_clicked();

signals:
    void ColorSelected(QColor);

private:
    Ui::ColorPicker *ui;
    QColor current_color;
};

#endif // COLORPICKER_H
