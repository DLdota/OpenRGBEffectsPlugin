#ifndef AMBIENT_H
#define AMBIENT_H

#include "RGBEffect.h"

#include <QVBoxLayout>
#include <QMainWindow>
#include <QMainWindow>
#include <QGuiApplication>
#include <QMainWindow>
#include <QScreen>
#include <QWindow>
#include <QWidget>

class Ambient : public RGBEffect
{
public:
    Ambient() {};
    ~Ambient() {};

    EffectInfo              DefineEffectDetails()                                       override;
    void                    DefineExtraOptions(QWidget*)                                override;
    void                    StepEffect(std::vector<OwnedControllerAndZones>, int FPS)   override;

    void                    SetSpeed(int)                           override {};
    void                    Slider2Changed(int)                     override {};
    void                    SetUserColors(std::vector<RGBColor>)    override {};
    void                    ASelectionWasChanged()                  override {};

    int                     GetSpeed()                              override {return 0;}  ;
    int                     GetSlider2Val()                         override {return 0;}  ;
    std::vector<RGBColor>   GetUserColors()                         override {return {};} ;
private:
    void GetScreenColor();

    QPixmap ScreenShot;

    EffectInfo EffectDetails;
};

#endif // AMBIENT_H
