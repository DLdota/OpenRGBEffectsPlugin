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

    EffectInfo EffectDetails;
private:
    int                     Speed;
    float                   Width;
    float                   Progress = 1;

    std::vector<RGBColor>   UserColors;
    hsv_t                   StartingColor;
    hsv_t                   EndingColor;


};

#endif // GRADIENTCYCLING_H
