#include "AudioVisualizer.h"

EffectInfo AudioVisualizer::DefineEffectDetails()
{

    EffectDetails.EffectName = "Audio Visualizer";
    EffectDetails.EffectDescription = "A modfied version of https://gitlab.com/CalcProgrammer1/KeyboardVisualizer";

    EffectDetails.IsReversable = false;
    EffectDetails.MaxSpeed = 0;
    EffectDetails.MinSpeed = 0;

    EffectDetails.UserColors = 0;
    EffectDetails.MaxSlider2Val = 0;
    EffectDetails.MinSlider2Val = 0;
    EffectDetails.Slider2Name = "";

    EffectDetails.HasCustomWidgets = true;
    EffectDetails.HasCustomSettings = true;

    return EffectDetails;
}

void AudioVisualizer::DefineExtraOptions(QLayout *Scaler)
{
    ADV_UI = new AudioVisualizerUi();
    Scaler->addWidget(ADV_UI);
}

void AudioVisualizer::StepEffect(std::vector<OwnedControllerAndZones> Controllers, int FPS)
{
    ADV_UI->Step(Controllers, FPS);
}

void AudioVisualizer::LoadCustomSettings(json Settings)
{
    ADV_UI->LoadSettings(Settings);
}

json AudioVisualizer::SaveCustomSettings(json)
{
    return ADV_UI->SaveSettings();
}

void AudioVisualizer::EffectState(bool IsRunning)
{
    ADV_UI->EffectState(IsRunning);
}
