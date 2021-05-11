#include "Ambient.h"

Ambient::Ambient() : RGBEffect()
{
    EffectDetails.EffectName = "Ambient";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Takes a portion of the screen and\nsets all of the LEDs to it";
    EffectDetails.IsReversable = false;

    EffectDetails.MaxSpeed = 0;
    EffectDetails.MinSpeed = 0;

    EffectDetails.MaxSlider2Val = 0;
    EffectDetails.MinSlider2Val = 0;
    EffectDetails.Slider2Name   = "";

    EffectDetails.UserColors = 0;

    EffectDetails.HasCustomWidgets = true;
    EffectDetails.HasCustomSettings = true;

    SCRNSLCT = new ScreenSelection();
}

Ambient::~Ambient()
{
    delete SCRNSLCT;
}

void Ambient::DefineExtraOptions(QLayout* ParentLayout)
{    
    ParentLayout->addWidget(SCRNSLCT);
}

void Ambient::StepEffect(std::vector<ControllerZone> controller_zones)
{
    SCRNSLCT->GetScreen();
    QColor C = SCRNSLCT->CalcColor();

    RGBColor color = ToRGBColor(C.red(),C.green(),C.blue());

    for (ControllerZone controller_zone : controller_zones)
    {
        controller_zone.controller->SetAllZoneLEDs(controller_zone.zone_idx, color);
    }
}

void Ambient::EffectState(bool Auto)
{
    SCRNSLCT->SetAuto(!Auto);
}

void Ambient::LoadCustomSettings(json Settings)
{
    SCRNSLCT->SetCalcType(Settings["CalcMode"]);
    SCRNSLCT->SetShowState(Settings["PreviewShowing"]);
}

json Ambient::SaveCustomSettings(json Settings)
{
    Settings["CalcMode"] = SCRNSLCT->GetCalcType();
    Settings["PreviewShowing"] = !SCRNSLCT->GetShowState();

    return Settings;
}
