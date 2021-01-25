#ifndef GRADIENTCYCLING_H
#define GRADIENTCYCLING_H

#include "RGBEffect.h"

class GradientCycling: public RGBEffect
{
public:
    GradientCycling() {};
    ~GradientCycling() {};

    EffectInfo  DefineEffectDetails()                                       override;
    void        DefineExtraOptions(QWidget* Parent)                         override;
    void        StepEffect(std::vector<OwnedControllerAndZones> LocalControllers, int Step)  override;

    void        SetSpeed(int Speed)                                         override;
    void        SetUserColors(std::vector<RGBColor>)                        override;
    void        Slider2Changed(int)                                         override;

    void        SetWidth(int NewWidth);
    void        ASelectionWasChanged() override;

    EffectInfo EffectDetails;
private:
    static std::vector<OwnedControllerAndZones> PrevControllers;

    int                     Speed;
    float                   Width;

    std::vector<std::vector<float>> Progress;
    std::vector<RGBColor>           UserColors;


};

#endif // GRADIENTCYCLING_H
