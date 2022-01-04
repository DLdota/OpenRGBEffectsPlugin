#ifndef Visor_H
#define Visor_H

#include "RGBEffect.h"
#include "EffectRegisterer.h"
#include "hsv.h"

class Visor: public RGBEffect
{
public:
    Visor();
    ~Visor() {};

    EFFECT_REGISTERER(ClassName(), CAT_SIMPLE, [](){return new Visor;});
    static std::string const ClassName() {return "Visor";}
    void StepEffect(std::vector<ControllerZone*>) override;

private:    
    float  Progress = 0.f;

    float  p = 0.;
    float  p_step =0.;
    bool   step = false;
    bool   last_step = false;
    float  width = 0.;
    float  h_width = 0.;

    RGBColor C0;
    RGBColor C1;

    RGBColor GetColor(float i, float count);
};

#endif // Visor_H

