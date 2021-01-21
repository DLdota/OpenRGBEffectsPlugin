#ifndef STARRYNIGHT_H
#define STARRYNIGHT_H

#include "RGBEffect.h"

class StarryNight: public RGBEffect
{
public:
    StarryNight() {};
    ~StarryNight() {};
    EffectInfo  DefineEffectDetails() override;
    void        DefineExtraOptions(QWidget* Parent) override;
    void        StepEffect(std::vector<OwnedControllerAndZones>, int Step) override;

    void        SetSpeed(int Speed) override;
    void        SetUserColors(std::vector<RGBColor>) override;
    void        Slider2Changed(int) override;

    EffectInfo          EffectDetails;
private:
    int Speed;
    int LEDPerCycle;
    std::vector<RGBColor> UserColors;
};

#endif // STARRYNIGHT_H
