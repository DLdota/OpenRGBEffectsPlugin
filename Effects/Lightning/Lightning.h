#ifndef LIGHTNING_H
#define LIGHTNING_H

#include <QWidget>
#include <stdlib.h>

#include "hsv.h"
#include "RGBEffect.h"

class Lightning: public QWidget, public RGBEffect
{
    Q_OBJECT

public:
    explicit Lightning(QWidget *parent = nullptr);
    ~Lightning(){}

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
