#include "OpenRGBEffectTab.h"
#include "OpenRGBEffectPage.h"


std::vector<RGBEffect*> OpenRGBEffectTab::ActiveEffects;
std::vector<BetterController> OpenRGBEffectTab::Controllers;

std::vector<RGBEffect*> OpenRGBEffectTab::EffectList;

/*-------------------------*\
| Define all of the effects |
\*-------------------------*/
void OpenRGBEffectTab::DefineEffects()
{
    SpectrumCycling *SpecCycle = new SpectrumCycling;
    OpenRGBEffectTab::EffectList.push_back(SpecCycle);

    RainbowWave * RBWave = new RainbowWave;
    OpenRGBEffectTab::EffectList.push_back(RBWave);
}


/*--------------------*\
| List entry Creation  |
\*--------------------*/
void OpenRGBEffectTab::CreateDeviceSelection(RGBController* Controller, int Index, bool HasDirectMode)
{
    int NewRow = ui->SelectDevices->rowCount();
    ui->SelectDevices->setRowCount(NewRow + 2);

    QTableWidgetItem* NewItem = new QTableWidgetItem(QString().fromStdString(Controller->name));
    if (!HasDirectMode)
    {
        NewItem->setForeground(Qt::red);
        NewItem->setToolTip("This device doesn't have direct mode\nUse at your own risk as this may damage the flash of your device");
    }
    ui->SelectDevices->setItem(NewRow,0,NewItem);

    QCheckBox* SelectedBox = new QCheckBox();
    ui->SelectDevices->setCellWidget(NewRow,1,SelectedBox);


    QCheckBox* ReversedBox = new QCheckBox();
    ui->SelectDevices->setCellWidget(NewRow,2,ReversedBox);

    QSignalMapper* SelectionMapper = new QSignalMapper(SelectedBox);
    SelectionMapper->setMapping(SelectedBox, (QString().fromStdString(Controller->name) + QString().number(Index)) );

    connect(SelectedBox,SIGNAL(clicked()),SelectionMapper,SLOT(map()));
    connect(SelectionMapper,SIGNAL(mappedString(QString)),this,SLOT( DeviceSelectionChanged(QString) ) );

    /*----------------------*\
    | Create the Zone table  |
    \*----------------------*/
    QTableWidget* ZoneTableChecks = new QTableWidget;

    ZoneTableChecks->setColumnCount(3);

    ZoneTableChecks->setFixedWidth(317);

    ZoneTableChecks->setEditTriggers(QTableWidget::NoEditTriggers);
    ZoneTableChecks->setSelectionMode(QTableWidget::NoSelection);
    ZoneTableChecks->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    std::vector<int> CollumnSizes = {165 , 75, 75};
    for (int i = 0; i < int(CollumnSizes.size()); i++)
    {
        ZoneTableChecks->setColumnWidth(i,CollumnSizes[i]);
    }

    ZoneTableChecks->verticalHeader()->hide();
    ZoneTableChecks->horizontalHeader()->hide();

    int RowHeight = 0;
    for (int ZoneNum = 0; ZoneNum < int(Controller->zones.size()); ZoneNum++)
    {
        ZoneTableChecks->setRowCount(ZoneTableChecks->rowCount() + 1);

        QTableWidgetItem* NewZoneName = new QTableWidgetItem(QString().fromStdString( ( "        " + Controller->zones[ZoneNum].name) ) ) ;

        if (!HasDirectMode)
        {
            NewZoneName->setForeground(Qt::red);
            NewZoneName->setToolTip("This device doesn't have direct mode\nUse at your own risk as this may damage the flash of your device");
        }

        ZoneTableChecks->setItem(ZoneNum, 0, NewZoneName);

        /*----------------------------------------------------------*\
        | Create Zone selection checkbox                             |
        | Connect the click signal to the SelectionChanged function  |
        | Push it to the nested TableWidget                          |
        \*----------------------------------------------------------*/
        QCheckBox* ZoneSelected = new QCheckBox();
        connect(ZoneSelected,SIGNAL(clicked()),this,SLOT(ZoneSelectionChanged()));

        ZoneTableChecks->setCellWidget(ZoneNum,1,ZoneSelected);
        RowHeight += 1;
    }

    ui->SelectDevices->setCellWidget((NewRow + 1),0,ZoneTableChecks);
    ui->SelectDevices->setRowHeight((NewRow + 1),(31*RowHeight) );
    return;
}


/*------------------------*\
| Contructor / Destructor  |
\*------------------------*/
OpenRGBEffectTab::OpenRGBEffectTab(QWidget *parent): QWidget(parent), ui(new Ui::OpenRGBEffectTab)
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

    connect(ui->LeftTabBar,SIGNAL(currentChanged(int)),this,SLOT(on_TabChange(int)));
    CurrentTab = 0;

    /*-----------------------*\
    | Make the Device view    |
    \*-----------------------*/
    ui->SelectDevices->setMinimumWidth(317);
    ui->SelectDevices->setColumnCount(3);
    ui->SelectDevices->setHorizontalHeaderLabels({"Device","Enabled","Reversed"});

    ui->SelectDevices->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    /*-------------------*\
    | Set collumn sizes   |
    \*-------------------*/
    std::vector<int> CollumnSizes = {165 , 75, 75};
    for (int i = 0; i < int(CollumnSizes.size()); i++)
    {
        ui->SelectDevices->setColumnWidth(i,CollumnSizes[i]);
    }

    /*------------------------------------------------*\
    | Register for callbacks and create effect thread  |
    \*------------------------------------------------*/
    ORGBPlugin::RMPointer->RegisterDeviceListChangeCallback(DeviceListChangedCallback, this);
    ORGBPlugin::RMPointer->RegisterDetectionProgressCallback(DeviceListChangedCallback, this);

    /*----------------------------------*\
    | Create the effect handling thread  |
    \*----------------------------------*/
    StepEffectThread = new std::thread(&OpenRGBEffectTab::EffectStepTimer,this);
}

OpenRGBEffectTab::~OpenRGBEffectTab()
{
    delete ui;
}


/*----------------------------------*\
| Set an Effect to Active/Unactive   |
| Effect Step thread function        |
\*----------------------------------*/
void OpenRGBEffectTab::SetEffectActive(RGBEffect* Effect)
{
    for (int i = 0; i < int(ActiveEffects.size()); i++)
    {
        if ( ActiveEffects[i] == Effect)
        {
            return;
        }
    }
    ActiveEffects.push_back(Effect);
}

void OpenRGBEffectTab::SetEffectUnActive(RGBEffect* Effect)
{
    for (int i = 0; i < int(ActiveEffects.size()); i++)
    {
        if (ActiveEffects[i] == Effect)
        {
            ActiveEffects.erase(ActiveEffects.begin() + i);
            return;
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
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));

                    std::vector<OwnedControllerAndZones> PassControllerList;

                    // For controller in the list of controllers
                    for (int ControllerID = 0; ControllerID < int(Controllers.size()); ControllerID++)
                    {
                        // Create and empty struct and set the controller object
                        OwnedControllerAndZones PassController;
                        PassController.Controller = Controllers[ControllerID].Controller;

                        // For zone in Controller's Zone list. If the controller is owned by the effect then add it to the list of owned zones
                        for (int ZoneID = 0; ZoneID < int(Controllers[ControllerID].OwnedZones.size()); ZoneID++)
                        {
                            if (Controllers[ControllerID].OwnedZones[ZoneID].EffectName == ActiveEffects[EffectIndex]->EffectDetails.EffectName)
                            {
                                PassController.OwnedZones.push_back(ZoneID);
                            }
                        }

                        // If the zones owned is greater than 0, Add it to the list of owned controllers
                        if (PassController.OwnedZones.size() != 0)
                        {
                            PassControllerList.push_back(PassController);
                        }

                    }

                    OpenRGBEffectTab::ActiveEffects[EffectIndex]->StepEffect(PassControllerList,EffectStep);
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

    /*--------------------------------------------------------*\
    | Grab new controllers and start making entries for them   |
    \*--------------------------------------------------------*/
    std::vector<RGBController*> NewControllers = ORGBPlugin::RMPointer->GetRGBControllers();
    for (int i = 0; i < int(NewControllers.size()); i++)
    {
        bool HasDirectMode = false;
        for (int ModeIndex = 0; ModeIndex < int(NewControllers[i]->modes.size()); ModeIndex++ )
        {
            if (NewControllers[i]->modes[ModeIndex].name == "Direct")
            {
                HasDirectMode = true;
                break;
            }
        }

        BetterController NewItem;
        NewItem.Controller  = NewControllers[i];
        NewItem.Index       = i;
        NewItem.HasDirect   = HasDirectMode;

        Controllers.push_back(NewItem);
    }

    /*---------------------*\
    | Add Devices to list   |
    \*---------------------*/
    ui->SelectDevices->setRowCount(0);
    for (int i = 0; i < int(Controllers.size()); i++)
    {
        CreateDeviceSelection(Controllers[i].Controller, Controllers[i].Index, Controllers[i].HasDirect);
    }
}


/*-------------------------*\
| Tab / Selection Handling  |
\*-------------------------*/
void OpenRGBEffectTab::DeviceSelectionChanged(QString DName)
{
    int DevIndex = 0;
    for (int DeviceID = 0; DeviceID < ui->SelectDevices->rowCount(); DeviceID++)
    {
        if (DeviceID%2) continue;
        int TempDevID = DeviceID/2;
        QTableWidgetItem* DeviceName = ui->SelectDevices->item(DeviceID,0);
        if ((DeviceName->text() + QString().number(Controllers[TempDevID].Index)) == DName)
        {
            qDebug() << DeviceName->text();
            DevIndex = DeviceID;
            break;
        }
        continue;
    }

    int DevTabIndex = DevIndex; // This is the index of the tab holding the device. It is mismatched because of the zone boxes
    DevIndex = DevIndex/2; // This is the actual index of the device

    QCheckBox* DeviceSelected = qobject_cast<QCheckBox* >(ui->SelectDevices->cellWidget(DevTabIndex,1));
    if ((DeviceSelected->isEnabled()) && (DeviceSelected->isChecked()))
    {
        QTableWidget* ZoneTable = qobject_cast<QTableWidget*>(ui->SelectDevices->cellWidget( (DevTabIndex + 1), 0));
        /*-----------------------------------------*\
        | Add all non owned zones to the list       |
        \*-----------------------------------------*/
        for (int ZoneID = 0; ZoneID < int(Controllers[DevIndex].Controller->zones.size()); ZoneID++)
        {
            QCheckBox* ZoneCheckBox = qobject_cast<QCheckBox*>(ZoneTable->cellWidget(ZoneID,1));
            if (ZoneCheckBox->isEnabled() && !ZoneCheckBox->isChecked())
            {
                ZoneOwnedBy Identifier;
                Identifier.EffectName = EffectList[CurrentTab]->EffectDetails.EffectName;
                Identifier.Zone = ZoneID;
                Controllers[DevIndex].OwnedZones.push_back(Identifier);

                ZoneCheckBox->setCheckState(Qt::Checked);
            }
        }
    }
    else if (DeviceSelected->isEnabled() && !DeviceSelected->isChecked())
    {
        QTableWidget* ZoneTable = qobject_cast<QTableWidget*>(ui->SelectDevices->cellWidget( (DevTabIndex + 1), 0));
        /*-----------------------------------------*\
        | Add all non owned zones to the list       |
        \*-----------------------------------------*/
        for (int ZoneID = 0; ZoneID < int(Controllers[DevIndex/2].Controller->zones.size()); ZoneID++)
        {
            QCheckBox* ZoneCheckBox = qobject_cast<QCheckBox*>(ZoneTable->cellWidget(ZoneID,1));
            if (ZoneCheckBox->isEnabled() && ZoneCheckBox->isChecked())
            {
                // Create this regardless so that we have something more specific to check against
                ZoneOwnedBy Identifier;
                Identifier.EffectName = EffectList[CurrentTab]->EffectDetails.EffectName;
                Identifier.Zone = ZoneID;

                for (int OwnedZoneIndex = 0; OwnedZoneIndex < int(Controllers[DevIndex].OwnedZones.size()); OwnedZoneIndex++)
                {
                    // This is just to have a shorter variable name
                    ZoneOwnedBy CheckAgainst = Controllers[DevIndex].OwnedZones[OwnedZoneIndex];
                    if ((CheckAgainst.EffectName == Identifier.EffectName) && (CheckAgainst.Zone == Identifier.Zone))
                    {
                        Controllers[DevIndex].OwnedZones.erase(Controllers[DevIndex].OwnedZones.begin() + OwnedZoneIndex);
                        break;
                    }
                }
                // Set the state to unchecked
                ZoneCheckBox->setCheckState(Qt::Unchecked);
            }
        }
    }
}

void OpenRGBEffectTab::ZoneSelectionChanged()
{
    //QCheckBox* CheckBox = qobject_cast<QCheckBox* >();
}

void OpenRGBEffectTab::on_TabChange(int Tab)
{
    OpenRGBEffectTab::CurrentTab = Tab;
}
