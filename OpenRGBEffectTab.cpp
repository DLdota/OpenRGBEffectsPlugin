#include "OpenRGBEffectTab.h"
#include "OpenRGBEffectPage.h"

std::vector<RGBController*> OpenRGBEffectTab::LockedControllers;
std::vector<RGBEffect*> OpenRGBEffectTab::EffectList;

void OpenRGBEffectTab::DefineEffects()
{
    SpectrumCycling *SpecCycle = new SpectrumCycling;
    OpenRGBEffectTab::EffectList.push_back(SpecCycle);
}

OpenRGBEffectTab::OpenRGBEffectTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OpenRGBEffectTab)
{
    ui->setupUi(this);

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

std::vector<RGBController*> OpenRGBEffectTab::LockControllers(std::vector<RGBController*> ToLock)
{
    std::vector<RGBController*> SuccessFullyLocked;
    for(int LockIndex = 0; LockIndex < int(ToLock.size()); LockIndex++)
    {
        bool FoundMatch = false;
        for (int i = 0; i < int(OpenRGBEffectTab::LockedControllers.size()); i++)
        {
            /*-----------------------------------------------------------------------------------*\
            | If any of the items in the devicelist are == to the controller trying to be locked  |
            \*-----------------------------------------------------------------------------------*/
            if(ToLock[LockIndex] == OpenRGBEffectTab::LockedControllers[i])
            {
                FoundMatch = true;
                break;
            }
        }
        if (!FoundMatch)
        {
            OpenRGBEffectTab::LockedControllers.push_back(ToLock[LockIndex]);
            SuccessFullyLocked.push_back(ToLock[LockIndex]);
        }
    }
    return SuccessFullyLocked;
}

void OpenRGBEffectTab::UnlockControllers(std::vector<RGBController*> ToUnlock)
{
    for(int UnlockIndex = 0; UnlockIndex < int(ToUnlock.size()); UnlockIndex++)
    {
        for (int i = 0; i < int(OpenRGBEffectTab::LockedControllers.size()); i++)
        {
            if(ToUnlock[UnlockIndex] == OpenRGBEffectTab::LockedControllers[i])
            {
                OpenRGBEffectTab::LockedControllers.erase(LockedControllers.begin()+UnlockIndex);
                break;
            }
        }
    }
}
