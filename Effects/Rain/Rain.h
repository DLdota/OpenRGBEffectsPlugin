#ifndef RAIN_H
#define RAIN_H

#include "RGBEffect.h"
#include "EffectRegisterer.h"

struct Drops
{
    int Index;

    zone_type   ZT;
    int   LEDCount;
    int   StartingLED;

    bool  Reversed;

    int    Column;
    int    ColumnCount;
    float  Progress;

    RGBColor C;
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
    std::vector<Drops> CurrentDrops;   
    std::vector<bool> HasEffect;

    int PrevNum;
};

#endif // RAIN_H
