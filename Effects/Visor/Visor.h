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

    EFFECT_REGISTERER(ClassName(), [](){return new Visor;});

    static std::string const ClassName() {return "Visor";}

    void StepEffect(std::vector<ControllerZone>) override;
    void SetUserColors(std::vector<RGBColor>) override;
    void SetSlider2Val(unsigned int) override;
    unsigned int GetSlider2Val() override;
    void SetRandomColorsEnabled(bool) override;

private:
    bool                    Dir = true;
    int                     width = 10;
    float                   Progress = 0;
    hsv_t                   Head;
    hsv_t                   Tail;
    float                   current_head_hue;
    float                   current_tail_hue;

    RGBColor GetColor(int i, int count);

    void GenerateRandomColors();
    void SetWidth(int);
};

#endif // Visor_H

