#ifndef AMBIENT_H
#define AMBIENT_H

#include <QWidget>
#include <QMouseEvent>
#include "ui_Ambient.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"
#include "RectangleSelector.h"
#include "ScreenRecorder.h"

namespace Ui {
class Ambient;
}

class Ambient : public RGBEffect
{
    Q_OBJECT

public:
    explicit Ambient(QWidget *parent = nullptr);
    ~Ambient();

    EFFECT_REGISTERER(ClassName(), CAT_SPECIAL, [](){return new Ambient;});

    static std::string const ClassName() {return "Ambient";}

    void StepEffect(std::vector<ControllerZone*>) override;
    void LoadCustomSettings(json) override;
    json SaveCustomSettings() override;
    void EffectState(bool) override;

private slots:
    void on_select_rectangle_clicked();
    void on_left_valueChanged(int);
    void on_top_valueChanged(int);
    void on_width_valueChanged(int);
    void on_height_valueChanged(int);
    void on_mode_currentIndexChanged(int);
    void on_screen_currentIndexChanged(int);

private:
    Ui::Ambient *ui;

    ScreenRecorder screen_recorder;

    int screen_index = -1;

    void UpdateSelection();

    RectangleSelectorOverlay* rectangle_selector_overlay;

    enum AmbientMode{
        SCALED_AVERAGE = 0,
        SCREEN_COPY = 1,
        CALCULATED_AVERAGE = 2,
        MOST_COMMON = 3
    };

    AmbientMode mode = CALCULATED_AVERAGE;

    RGBColor old_single_color = ToRGBColor(0,0,0);
    RGBColor Smooth(RGBColor color);

    int left = 0;
    int top = 0;
    int width = 1;
    int height = 1;
};

#endif // AMBIENT_H
