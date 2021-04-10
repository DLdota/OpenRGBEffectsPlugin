#include "Ambient.h"

EffectInfo Ambient::DefineEffectDetails()
{
    Ambient::EffectDetails.EffectName = "Ambient";
    Ambient::EffectDetails.EffectDescription = "Takes a portion of the screen and\nsets all of the LEDs to it";
    Ambient::EffectDetails.IsReversable = false;

    Ambient::EffectDetails.MaxSpeed = 0;
    Ambient::EffectDetails.MinSpeed = 0;

    Ambient::EffectDetails.MaxSlider2Val = 0;
    Ambient::EffectDetails.MinSlider2Val = 0;
    Ambient::EffectDetails.Slider2Name   = "";

    Ambient::EffectDetails.UserColors = 0;

    Ambient::EffectDetails.HasCustomWidgets = true;
    Ambient::EffectDetails.HasCustomSettings = true;

    return Ambient::EffectDetails;
}

void Ambient::DefineExtraOptions(QLayout* ParentLayout)
{
    ScreenSelection* ImgPrev = new ScreenSelection();
    SCRNSLCT = ImgPrev;
    ParentLayout->addWidget(ImgPrev);
}

void Ambient::StepEffect(std::vector<OwnedControllerAndZones> Controllers, int)
{
    SCRNSLCT->GetScreen();
    QColor C = SCRNSLCT->CalcColor();

    RGBColor SetCol = ToRGBColor(C.red(),C.green(),C.blue());

    for (int ControllerID = 0; ControllerID < (int)Controllers.size(); ControllerID++)
    {
        for (int ZoneID = 0; ZoneID < (int)Controllers[ControllerID].OwnedZones.size(); ZoneID++)
        {
            Controllers[ControllerID].Controller->SetAllZoneLEDs(Controllers[ControllerID].OwnedZones[ZoneID],SetCol);
        }
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
