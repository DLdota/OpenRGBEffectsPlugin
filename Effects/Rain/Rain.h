#ifndef RAIN_H
#define RAIN_H

#include "RGBEffect.h"
#include "EffectRegisterer.h"

struct Drop
{
    double progress;
    RGBColor color;
    unsigned int col;
    float speed_mult;
};

class Rain: public RGBEffect
{
public:
    Rain();
    ~Rain(){}

    EFFECT_REGISTERER(ClassName(), [](){return new Rain;});

    static std::string const ClassName() { return "Rain"; }

    void StepEffect(std::vector<ControllerZone>) override;
    void ASelectionWasChanged(std::vector<ControllerZone>) override;

private:
    RGBColor GetColor(unsigned int, unsigned int, unsigned int);
    void  TriggerDrop(unsigned int, unsigned int);
    void  CleanDrops(unsigned int, unsigned int);
    void  RunDrops(unsigned int);
    RGBColor Enlight(float, RGBColor);

    std::vector<std::vector<Drop>> drops;
    RGBColor OFF = ToRGBColor(0,0,0);

};

#endif // RAIN_H
