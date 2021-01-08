#include "OpenRGBEffectTab.h"
#include "ui_OpenRGBEffectTab.h"

#include "SpectrumCycling.h"

std::vector<RGBEffect> OpenRGBEffectTab::EffectList =
{
    SpectrumCycling()
};

OpenRGBEffectTab::OpenRGBEffectTab(QWidget *parent, ResourceManager* RMPointer) :
    QWidget(parent),
    ui(new Ui::OpenRGBEffectTab)
{
    ui->setupUi(this);
    RGBControllerList = RMPointer->GetRGBControllers();
}

OpenRGBEffectTab::~OpenRGBEffectTab()
{
    delete ui;
}
