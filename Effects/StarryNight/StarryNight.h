#ifndef STARRYNIGHT_H
#define STARRYNIGHT_H

#include "RGBEffect.h"
#include <stdlib.h>
#include <hsv.h>

struct NewStar
{
    int         Index;
    int         LED;
    float       state;
    RGBColor    Color;
};

class StarryNight: public RGBEffect
{
public:
    StarryNight();
    ~StarryNight() {};

    static std::string const ClassName() {return "StarryNight";}

    void StepEffect(std::vector<ControllerZone>) override;
    void ASelectionWasChanged(std::vector<ControllerZone>) override;

private:
    std::vector<NewStar> CurrentStars;
};

#endif // STARRYNIGHT_H
