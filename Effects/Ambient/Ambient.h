#ifndef AMBIENT_H
#define AMBIENT_H

#include "RGBEffect.h"
#include "ScreenSelection.h"

class Ambient : public RGBEffect
{
public:
    Ambient();
    ~Ambient();

    static std::string const ClassName() {return "Ambient";}

    void                    DefineExtraOptions(QLayout*)                                override;
    void                    StepEffect(std::vector<ControllerZone>)                     override;
    void                    LoadCustomSettings(json)                                    override;
    json                    SaveCustomSettings(json)                                    override;
    void                    EffectState(bool)                                           override;

private:
    ScreenSelection* SCRNSLCT;
};

#endif // AMBIENT_H
