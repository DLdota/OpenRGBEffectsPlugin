#ifndef RAINBOWWAVE_H
#define RAINBOWWAVE_H

#include "RGBEffect.h"
#include "OpenRGBEffectPage.h"

class RainbowWave: public RGBEffect
{
public:
    RainbowWave() {};
    ~RainbowWave() {};

    EffectInfo  DefineEffectDetails()                                       override;
    void        DefineExtraOptions(QWidget* Parent)                         override;
    void        StepEffect(std::vector<OwnedControllerAndZones> LocalControllers, int Step)  override;

    void        SetSpeed(int Speed)                                         override;
    void        SetUserColors(std::vector<RGBColor>)                        override;
    void        Slider2Changed(int)                                         override;

    void        SetWidth(int NewWidth);

    EffectInfo EffectDetails;
private:
    int     Speed;
    int     Width;
    float   Progress = 0;
};

#endif // RAINBOWWAVE_H