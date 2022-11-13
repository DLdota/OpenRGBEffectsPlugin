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
    void SetPreviewButtonVisible(bool);
    void SetFPSSliderVisible(bool);

signals:
    void EffectState(bool);

private slots:
    void on_preview_clicked();
    void on_SpeedSlider_valueChanged(int value);
    void on_Slider2_valueChanged(int value);
    void on_FPS_slider_valueChanged(int value);
    void on_Brightness_slider_valueChanged(int value);
    void on_RandomCheckbox_clicked();
    void on_OnlyFirst_clicked();    
    void on_toggle_info_clicked();

    void SavePatternAction();
    void LoadPatternAction();
    void EditPatternAction();
    void OpenPatternsFolder();

private:
    Ui::OpenRGBEffectPage *ui;

    RGBEffect* effect;

    std::vector<RGBController*> OwnedController;
    std::vector<RGBColor> UserColors;
    std::vector<ColorPicker*> ColorPickers;

    bool RandomColors = false;

    void InitUi();
    void OpenPreview();

    QDialog* preview_dialog;

    ControllerZone* preview_zone;

    QHBoxLayout* colors_layout = nullptr;    

    json ToJson();

    bool info_visible = false;

    void ApplyJson(json);

    void ToggleInfo();
};

#endif // OPENRGBEFFECTPAGE_H
