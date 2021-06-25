#ifndef LIGHTNING_H
#define LIGHTNING_H

#include <QWidget>
#include <stdlib.h>

#include "hsv.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"

class Lightning: public RGBEffect
{
    Q_OBJECT

public:
    explicit Lightning(QWidget *parent = nullptr);
    ~Lightning(){}

    EFFECT_REGISTERER(ClassName(), [](){return new Lightning;});

    static std::string const ClassName() {return "Lightning";}

    void StepEffect(std::vector<ControllerZone>) override;
    void SetUserColors(std::vector<RGBColor>) override;
    void ASelectionWasChanged(std::vector<ControllerZone>) override;

private:
    hsv_t UserHSV;
    std::vector<hsv_t> Lightnings;

    RGBColor TriggerLightning(int);
};

#endif // LIGHTNING_H
