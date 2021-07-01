#ifndef OPENRGBEFFECTPAGE_H
#define OPENRGBEFFECTPAGE_H

#include "ui_OpenRGBEffectPage.h"

#include "RGBEffect.h"
#include "ColorPicker.h"

namespace Ui {
class OpenRGBEffectPage;
}

class OpenRGBEffectPage : public QWidget
{
    Q_OBJECT

public:
    explicit OpenRGBEffectPage(QWidget *parent = nullptr, RGBEffect* EFCT = nullptr);
    ~OpenRGBEffectPage();

    void CreateDeviceSelection(std::string DeviceName);

    RGBEffect* GetEffect();

    void StartEffect();
    void StopEffect();

signals:
    void EffectState(bool);

private slots:
    void on_StartButton_clicked();
    void on_StopButton_clicked();
    void on_PreviewButton_clicked();
    void on_SpeedSlider_valueChanged(int value);
    void on_Slider2_valueChanged(int value);
    void on_FPS_slider_valueChanged(int value);
    void on_AutoStart_clicked();
    void on_RandomCheckbox_clicked();
    void on_OnlyFirst_clicked();
    void on_save_pattern_clicked();
    void on_load_pattern_clicked();
    void on_edit_pattern_clicked();

private:
    Ui::OpenRGBEffectPage *ui;

    RGBEffect* effect;

    std::vector<RGBController*> OwnedController;
    std::vector<RGBColor> UserColors;
    std::vector<ColorPicker*> ColorPickers;

    bool AutoStart = false;
    bool RandomColors = false;

    void InitUi();
    void OpenPreview();

    std::vector<unsigned int> speeds;

    QDialog* preview_dialog;

    ControllerZone* preview_zone;

    QHBoxLayout* colors_layout = nullptr;    

    json ToJson();
    void ApplyJson(json);
};

#endif // OPENRGBEFFECTPAGE_H
