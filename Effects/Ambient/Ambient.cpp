#include "Ambient.h"

EffectInfo Ambient::DefineEffectDetails()
{
    Ambient::EffectDetails.EffectName = "Ambient";
    Ambient::EffectDetails.EffectDescription = "Takes a portion of the screen and\nsets all of the LED to it";
    Ambient::EffectDetails.IsReversable = false;

    Ambient::EffectDetails.MaxSpeed = 0;
    Ambient::EffectDetails.MinSpeed = 0;

    Ambient::EffectDetails.MaxSlider2Val = 0;
    Ambient::EffectDetails.MinSlider2Val = 0;
    Ambient::EffectDetails.Slider2Name   = "";

    Ambient::EffectDetails.UserColors = 0;

    return Ambient::EffectDetails;
}

void Ambient::GetScreenColor()
{
    QScreen* Screen = QGuiApplication::primaryScreen();

    ScreenShot = Screen->grabWindow(0);
}

void Ambient::DefineExtraOptions(QWidget* Parent)
{
    /*QVBoxLayout* imgPrevLayout = new QVBoxLayout(Parent);
    const QRect screenGeometry = Parent->windowHandle()->geometry();

    GetScreenColor();*/
    //resize(300, 200);
}

void Ambient::StepEffect(std::vector<OwnedControllerAndZones> Controllers, int)
{
    GetScreenColor();
    QColor QRGBCol = ScreenShot.toImage().pixelColor(100,100);
    RGBColor SetCol = ToRGBColor(QRGBCol.red(), QRGBCol.green(), QRGBCol.blue());
    for (int ControllerID = 0; ControllerID < (int)Controllers.size(); ControllerID++)
    {
        for (int ZoneID = 0; ZoneID < (int)Controllers[ControllerID].OwnedZones.size(); ZoneID++)
        {
            Controllers[ControllerID].Controller->SetAllZoneLEDs(Controllers[ControllerID].OwnedZones[ZoneID],SetCol);
        }
    }
}
