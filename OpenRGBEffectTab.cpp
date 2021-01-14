#include "OpenRGBEffectTab.h"
#include "OpenRGBEffectPage.h"


std::vector<RGBEffect*> OpenRGBEffectTab::ActiveEffects;
std::vector<BetterController> OpenRGBEffectTab::Controllers;

std::vector<EffectInstStruct> OpenRGBEffectTab::EffectList;

void OpenRGBEffectTab::DefineEffects()
{
    EffectInstStruct SpecInst;
    SpectrumCycling *SpecCycle = new SpectrumCycling;
    SpecInst.EffectInst = SpecCycle;
    OpenRGBEffectTab::EffectList.push_back(SpecInst);

    EffectInstStruct RBWaveInst;
    RainbowWave * RBWave = new RainbowWave;
    RBWaveInst.EffectInst = RBWave;
    OpenRGBEffectTab::EffectList.push_back(RBWaveInst);
}

void OpenRGBEffectTab::CreateDeviceSelection(std::string DeviceName)
{
    int NewRow = ui->SelectDevices->rowCount();
    ui->SelectDevices->setRowCount(NewRow + 1);

    QTableWidgetItem* NewItem = new QTableWidgetItem(QString().fromStdString(DeviceName));
    ui->SelectDevices->setItem(NewRow,0,NewItem);

    QCheckBox* SelectedBox = new QCheckBox();
    ui->SelectDevices->setCellWidget(NewRow,1,SelectedBox);


    QCheckBox* ReversedBox = new QCheckBox();
    ui->SelectDevices->setCellWidget(NewRow,2,ReversedBox);

    connect(SelectedBox,SIGNAL(clicked()),this,SLOT(DeviceSelectionChanged()));
    return;
}

OpenRGBEffectTab::OpenRGBEffectTab(QWidget *parent):
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
        EffectList[i].EffectInst->EffectDetails = EffectList[i].EffectInst->DefineEffectDetails();

        /*--------------------*\
        | Make the label       |
        \*--------------------*/
        QLabel* EffectTabLabel = new QLabel();
        EffectTabLabel->setText(QString().fromStdString(EffectList[i].EffectInst->EffectDetails.EffectName));
        EffectTabLabel->setIndent(20);
        if(ORGBPlugin::DarkTheme)
        {
            EffectTabLabel->setGeometry(0, 25, 200, 50);
        }
        else
        {
            EffectTabLabel->setGeometry(0, 0, 200, 25);
        }
        OpenRGBEffectPage* EffectPage = new OpenRGBEffectPage(nullptr,EffectList[i].EffectInst);
        ui->LeftTabBar->addTab(EffectPage,"");
        ui->LeftTabBar->tabBar()->setTabButton(ui->LeftTabBar->count() -1, QTabBar::LeftSide,EffectTabLabel);

        connect(ui->LeftTabBar,SIGNAL(currentChanged(int)),this,SLOT(on_TabChange(int)));
        CurrentTab = 0;
        /*
        | Make the Device view
        */
        ui->SelectDevices->setMinimumWidth(317);
        ui->SelectDevices->setColumnCount(3);
        ui->SelectDevices->setHorizontalHeaderLabels({"Device","Enabled","Reversed"});
        /*-------------------*\
        | Set collumn sizes   |
        \*-------------------*/
        std::vector<int> CollumnSizes = {165 , 75, 75};
        for (int i = 0; i < int(CollumnSizes.size()); i++)
        {
            ui->SelectDevices->setColumnWidth(i,CollumnSizes[i]);
        }
        ORGBPlugin::RMPointer->RegisterDeviceListChangeCallback(DeviceListChangedCallback, this);
        ORGBPlugin::RMPointer->RegisterDetectionProgressCallback(DeviceListChangedCallback, this);
    }
}

OpenRGBEffectTab::~OpenRGBEffectTab()
{
    delete ui;
}

/*----------------------------------*\
| Set an Effect to Active/Unactive   |
\*----------------------------------*/
void OpenRGBEffectTab::SetEffectActive(RGBEffect* Effect)
{

}

void OpenRGBEffectTab::SetEffectUnActive(RGBEffect *Effect)
{

}

/*
| Device list change handling |
*/
void OpenRGBEffectTab::DeviceListChangedCallback(void* ptr)
{
    OpenRGBEffectTab * this_obj = (OpenRGBEffectTab *)ptr;

    QMetaObject::invokeMethod(this_obj, "DeviceListChanged", Qt::QueuedConnection);
}

void OpenRGBEffectTab::DeviceListChanged()
{
    /*-----------------------------*\
    | Wipe the list of controllers  |
    \*-----------------------------*/
    OpenRGBEffectTab::Controllers.erase(OpenRGBEffectTab::Controllers.begin(),OpenRGBEffectTab::Controllers.end());

    /*--------------------------------------------------------*\
    | Grab new controllers and start making entries for them   |
    \*--------------------------------------------------------*/
    std::vector<RGBController*> NewControllers = ORGBPlugin::RMPointer->GetRGBControllers();
    for (int i = 0; i < int(NewControllers.size()); i++)
    {
        BetterController NewItem;
        NewItem.Controller  = NewControllers[i];
        NewItem.Index       = i;
        NewItem.Locked      = false;

        OpenRGBEffectTab::Controllers.push_back(NewItem);
    }

    /*---------------------*\
    | Add Devices to list   |
    \*---------------------*/
    ui->SelectDevices->setRowCount(0);
    for (int i = 0; i < int(OpenRGBEffectTab::Controllers.size()); i++)
    {
        CreateDeviceSelection(OpenRGBEffectTab::Controllers[i].Controller->name);
    }
}

void OpenRGBEffectTab::DeviceSelectionChanged()
{
    for (int i = 0; i < ui->SelectDevices->rowCount(); i++)
    {
        /*-----------------------------------------------------*\
        | For row in table widget                               |
        |   If checkbox is checked                              |
        |       Add the Device to the list that will be locked  |
        \*-----------------------------------------------------*/
        QCheckBox *Selectedbox = qobject_cast<QCheckBox *>(ui->SelectDevices->cellWidget(i,1));
        if ((Selectedbox->isChecked()) && (Selectedbox->isEnabled()) )
        {
            EffectList[CurrentTab].OwnedControllers.push_back(Controllers[i].Controller);
            Controllers[i].Locked = true;
            Controllers[i].OwnedBy = EffectList[CurrentTab].EffectInst->EffectDetails.EffectName;
        }
        else if (!(Selectedbox->isChecked()) && (Selectedbox->isEnabled()))
        {
            if (EffectList[CurrentTab].OwnedControllers.size() > 0)
            {
                for (int CheckNoLongerOwned = 0; CheckNoLongerOwned < int(EffectList[CurrentTab].OwnedControllers.size()); CheckNoLongerOwned++)
                {
                    if (Controllers[i].Controller == EffectList[CurrentTab].OwnedControllers[CheckNoLongerOwned])
                    {
                        EffectList[CurrentTab].OwnedControllers.erase(EffectList[CurrentTab].OwnedControllers.begin() + CheckNoLongerOwned);
                    }
                }
            }
            Controllers[i].Locked = false;
            Controllers[i].OwnedBy = "";
        }
    }
}

void OpenRGBEffectTab::on_TabChange(int Tab)
{
    OpenRGBEffectTab::CurrentTab = Tab;
    for (int ControllerIndex = 0; ControllerIndex < int(Controllers.size()); ControllerIndex++)
    {
        /*---------------------------------------------------------------*\
        | If it is locked and it is not owned by the current tab then...  |
        \*---------------------------------------------------------------*/
        if ((Controllers[ControllerIndex].Locked) && !(Controllers[ControllerIndex].OwnedBy == EffectList[CurrentTab].EffectInst->EffectDetails.EffectName))
        {
            ui->SelectDevices->cellWidget(Controllers[ControllerIndex].Index, 1)->setDisabled(true);
            ui->SelectDevices->cellWidget(Controllers[ControllerIndex].Index, 1)->setToolTip(QString().fromStdString("In use by " + Controllers[ControllerIndex].OwnedBy));

            ui->SelectDevices->cellWidget(Controllers[ControllerIndex].Index, 2)->setDisabled(true);
            ui->SelectDevices->cellWidget(Controllers[ControllerIndex].Index, 2)->setToolTip(QString().fromStdString("In use by " + Controllers[ControllerIndex].OwnedBy));
        }
        else
        {
            ui->SelectDevices->cellWidget(Controllers[ControllerIndex].Index, 1)->setDisabled(false);
            ui->SelectDevices->cellWidget(Controllers[ControllerIndex].Index, 1)->setToolTip("");

            ui->SelectDevices->cellWidget(Controllers[ControllerIndex].Index, 2)->setDisabled(false);
            ui->SelectDevices->cellWidget(Controllers[ControllerIndex].Index, 2)->setToolTip("");
        }
    }
}
