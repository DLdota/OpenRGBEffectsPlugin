#ifndef GRADIENTWAVE_H
#define GRADIENTWAVE_H

#include "RGBEffect.h"

class GradientWave: public RGBEffect
{
public:
    GradientWave();
    ~GradientWave(){}

    static std::string const ClassName() { return "GradientWave"; }

    void StepEffect(std::vector<ControllerZone>) override;
    void SetRandomColorsEnabled(bool) override;
    void ASelectionWasChanged(std::vector<ControllerZone>) override;

private:
    std::vector<float> Progress;
    RGBColor RandomColorList[2];
};

#endif // GRADIENTWAVE_H
