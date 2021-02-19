#ifndef AUDIOVISUALIZER_H
#define AUDIOVISUALIZER_H

#include "RGBEffect.h"
#include "AudioVisualizerUi.h"
#include "Visualizer.h"

class AudioVisualizer : public RGBEffect
{
public:
    AudioVisualizer() {};
    EffectInfo  DefineEffectDetails()               override;
    void        DefineExtraOptions(QLayout* Scaler) override;
    void        StepEffect(std::vector<OwnedControllerAndZones>, int FPS) override;

    void        SetSpeed(int)                        override {};
    void        SetUserColors(std::vector<RGBColor>) override {};
    void        Slider2Changed(int)                  override {};
    void        ASelectionWasChanged(std::vector<OwnedControllerAndZones>) override {};
    void        ToggleRandomColors(bool)             override {};

    int                     GetSpeed()               override {return 0; };
    int                     GetSlider2Val()          override {return 0; };
    std::vector<RGBColor>   GetUserColors()          override {return {};};

    void                    EffectState(bool)        override;

    void                    LoadCustomSettings(json) override;
    json                    SaveCustomSettings(json) override;

    EffectInfo  EffectDetails;
private:
    AudioVisualizerUi* ADV_UI;
};

#endif // AUDIOVISUALIZER_H
