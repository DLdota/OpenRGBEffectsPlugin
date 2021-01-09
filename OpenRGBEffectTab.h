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
    explicit OpenRGBEffectTab(QWidget *parent = nullptr, ResourceManager* RMPointer = nullptr);
    ~OpenRGBEffectTab();

    static std::vector<RGBController*> RGBControllerList;
    void DefineEffects();
private:
    Ui::OpenRGBEffectTab            *ui;
    static std::vector<RGBEffect*>   EffectList;
};

#endif // OPENRGBEFFECTTAB_H
