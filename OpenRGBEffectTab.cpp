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

        /*-----------------------*\
        | Make the Device view    |
        \*-----------------------*/
        ui->SelectDevices->setMinimumWidth(317);
        ui->SelectDevices->setColumnCount(3);
        ui->SelectDevices->setHorizontalHeaderLabels({"Device","Enabled","Reversed"});

        std::vector<int> CollumnSizes = {165 , 75, 75};
        for (int i = 0; i < int(CollumnSizes.size()); i++)
        {
            /*-------------------*\
            | Set collumn sizes   |
            \*-------------------*/
            ui->SelectDevices->setColumnWidth(i,CollumnSizes[i]);
        }

        /*------------------------------------------------*\
        | Register for callbacks and create effect thread  |
        \*------------------------------------------------*/
        ORGBPlugin::RMPointer->RegisterDeviceListChangeCallback(DeviceListChangedCallback, this);
        ORGBPlugin::RMPointer->RegisterDetectionProgressCallback(DeviceListChangedCallback, this);

        StepEffectThread = new std::thread(&OpenRGBEffectTab::EffectStepTimer,this);
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
    ActiveEffects.push_back(Effect);
}

void OpenRGBEffectTab::SetEffectUnActive(RGBEffect *Effect)
{
    for (int i = 0; i < int(ActiveEffects.size()); i++)
    {
        if (ActiveEffects[i] == Effect)
        {
            ActiveEffects.erase(ActiveEffects.begin() + i);
        }
    }
}

void OpenRGBEffectTab::EffectStepTimer()
{
    std::thread([=]()
    {
        int EffectStep = 1;
        while (true) {
            if (int(OpenRGBEffectTab::ActiveEffects.size()) > 0)
            {
                for (int EffectIndex = 0; EffectIndex < int(OpenRGBEffectTab::ActiveEffects.size()); EffectIndex++)
                {
                    OpenRGBEffectTab::EffectList[EffectIndex].EffectInst->StepEffect(OpenRGBEffectTab::EffectList[EffectIndex].OwnedControllers,EffectStep);
                }

                /*---------------------------*\
                | Progress the Step counter   |
                \*---------------------------*/
                if (EffectStep < 10)
                {
                    EffectStep += 1;
                }
                else if (EffectStep >= 10)
                {
                    EffectStep = 1;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(16)); // 60 FPS
            }
        }
    }).detach();
}


/*---------------------------*\
| Device list change handling |
\*---------------------------*/
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
    Controllers.erase(Controllers.begin(),Controllers.end());

    /*---------------------------------------------------------------------------------------------*\
    | Empty out the owned controllers                                                               |
    | This may need a rework at some point so that the previously owned controllers are still owned |
    \*---------------------------------------------------------------------------------------------*/
    for (int i = 0; i < int(EffectList.size()); i++)
    {
        EffectList[i].OwnedControllers.erase(EffectList[i].OwnedControllers.begin(),EffectList[i].OwnedControllers.end());
    }

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

        Controllers.push_back(NewItem);
    }

    /*---------------------*\
    | Add Devices to list   |
    \*---------------------*/
    ui->SelectDevices->setRowCount(0);
    for (int i = 0; i < int(Controllers.size()); i++)
    {
        CreateDeviceSelection(Controllers[i].Controller->name);
    }
}


/*-------------------------*\
| Tab / Selection Handling  |
\*-------------------------*/
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
