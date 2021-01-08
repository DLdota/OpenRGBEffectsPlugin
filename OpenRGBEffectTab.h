#ifndef OPENRGBEFFECTTAB_H
#define OPENRGBEFFECTTAB_H

#include <QWidget>
#include "ORGBEffectPlugin.h"
#include "RGBEffect.h"
namespace Ui {
class OpenRGBEffectTab;
}

class OpenRGBEffectTab : public QWidget
{
    Q_OBJECT

public:
    explicit OpenRGBEffectTab(QWidget *parent = nullptr, ResourceManager* RMPointer = nullptr);
    ~OpenRGBEffectTab();

    std::vector<RGBController*> RGBControllerList;

private:
    Ui::OpenRGBEffectTab            *ui;
    static std::vector<RGBEffect>   EffectList;
};

#endif // OPENRGBEFFECTTAB_H
