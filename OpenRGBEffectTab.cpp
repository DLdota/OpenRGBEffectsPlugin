#include "OpenRGBEffectTab.h"
#include "OpenRGBEffectPage.h"

std::vector<RGBEffect*> OpenRGBEffectTab::EffectList;

void OpenRGBEffectTab::DefineEffects()
{
    SpectrumCycling *SpecCycle = new SpectrumCycling;
    OpenRGBEffectTab::EffectList.push_back(SpecCycle);
}

OpenRGBEffectTab::OpenRGBEffectTab(QWidget *parent, ResourceManager* RMPointer) :
    QWidget(parent),
    ui(new Ui::OpenRGBEffectTab)
{
    ui->setupUi(this);
    RGBControllerList = RMPointer->GetRGBControllers();

    DefineEffects();
    for (int i = 0; i < int(EffectList.size()); i++)
    {
        /*--------------------------------*\
        | Fill in the details              |
        \*--------------------------------*/
        EffectList[i]->EffectDetails = EffectList[i]->DefineEffectDetails();

        /*--------------------*\
        | Make the label       |
        \*--------------------*/
        QLabel* EffectTabLabel = new QLabel();
        EffectTabLabel->setText(QString().fromStdString(EffectList[i]->EffectDetails.EffectName));
        EffectTabLabel->setIndent(20);
        if(ORGBPlugin::DarkTheme)
        {
            EffectTabLabel->setGeometry(0, 25, 200, 50);
        }
        else
        {
            EffectTabLabel->setGeometry(0, 0, 200, 25);
        }
        OpenRGBEffectPage* EffectPage = new OpenRGBEffectPage(nullptr,EffectList[i]);
        ui->LeftTabBar->addTab(EffectPage,"");
        ui->LeftTabBar->tabBar()->setTabButton(ui->LeftTabBar->count() -1, QTabBar::LeftSide,EffectTabLabel);

    }

}

OpenRGBEffectTab::~OpenRGBEffectTab()
{
    delete ui;
}
