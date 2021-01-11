#ifndef OPENRGBEFFECTTAB_H
#define OPENRGBEFFECTTAB_H

#include <QWidget>
#include "ui_OpenRGBEffectTab.h"
#include "ORGBEffectPlugin.h"

#include "RGBEffect.h"
#include "SpectrumCycling.h"

namespace Ui {
class OpenRGBEffectTab;
}

class OpenRGBEffectTab : public QWidget
{
    Q_OBJECT

public:
    explicit OpenRGBEffectTab(QWidget *parent = nullptr);
    ~OpenRGBEffectTab();

    void DefineEffects();

    static std::vector<RGBController*> LockControllers(std::vector<RGBController*>);
    static void                        UnlockControllers(std::vector<RGBController*>);

private:
    Ui::OpenRGBEffectTab                *ui;
    static std::vector<RGBEffect*>      EffectList;
    static std::vector<RGBController*>  LockedControllers;
};

#endif // OPENRGBEFFECTTAB_H
