#include "OpenRGBEffectTab.h"
#include "OpenRGBEffectPage.h"


std::vector<RGBEffect*>                             OpenRGBEffectTab::ActiveEffects;
std::vector<BetterController>                       OpenRGBEffectTab::Controllers;
std::vector<std::vector<OwnedControllerAndZones>>   OpenRGBEffectTab::RespectiveToPass;

std::vector<RGBEffect*> OpenRGBEffectTab::EffectList;

std::vector<int> OpenRGBEffectTab::GetSpeed = {1,2,3,4,5,6,7,8,10,15,20,25,30,40,50,60};

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

    // Map the button press to send a string with the device name and index to the function
    QSignalMapper* DeviceSelectionMapper = new QSignalMapper(SelectedBox);
    DeviceSelectionMapper->setMapping(SelectedBox, (QString().fromStdString(Controller->name) + QString().number(Index)) );

    connect(SelectedBox,SIGNAL(clicked()),DeviceSelectionMapper,SLOT(map()));
    connect(DeviceSelectionMapper,SIGNAL(mappedString(QString)),this,SLOT( DeviceSelectionChanged(QString) ) );

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

    if (int(Controller->zones.size()) > 1)
    {
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

            // Same mapping, Just to a different function
            QSignalMapper* ZoneSelectionMapper = new QSignalMapper(ZoneSelected);
            ZoneSelectionMapper->setMapping(ZoneSelected, (QString().fromStdString(Controller->name) + QString().number(Index)) );

            connect(ZoneSelected,SIGNAL(clicked()),ZoneSelectionMapper,SLOT(map()));
            connect(ZoneSelectionMapper,SIGNAL(mappedString(QString)),this,SLOT( ZoneSelectionChanged(QString) ) );

            ZoneTableChecks->setCellWidget(ZoneNum,1,ZoneSelected);
            RowHeight += 1;
        }
        ui->SelectDevices->setRowHeight((NewRow + 1),(31*RowHeight) );
    }
    else if (int(Controller->zones.size()) == 1)
    {
        ZoneTableChecks->setRowCount(0);
        ZoneTableChecks->setFixedHeight(0);
        ui->SelectDevices->setRowHeight((NewRow + 1),0);
    }
    ui->SelectDevices->setCellWidget((NewRow + 1),0,ZoneTableChecks);
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
        EffectList[i]->EffectDetails.EffectIndex = i;

        std::vector<OwnedControllerAndZones> BlankStarter;
        RespectiveToPass.push_back(BlankStarter);

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


    OpenRGBEffectTab::FPS = 1000;
    ui->FPSCount->setText(QString().number(1));

    ui->FPSSlider->setMaximum(int(OpenRGBEffectTab::GetSpeed.size()) - 1);
    ui->FPSSlider->setMinimum(0);
    connect(ui->FPSSlider,SIGNAL(valueChanged(int)),this,SLOT(FPSSlider(int)));

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
                    OpenRGBEffectTab::ActiveEffects[EffectIndex]->StepEffect(RespectiveToPass[EffectIndex],EffectStep);
                }

                // After running through all of the effects proceed to set all of the LEDs on a per zone basis
                for (int ControllerID = 0; ControllerID < int(Controllers.size()); ControllerID++)
                {
                    if (Controllers[ControllerID].OwnedZones.size() > 0)
                    {
                        Controllers[ControllerID].Controller->UpdateLEDs();
                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    }
                }

                /*---------------------------*\
                | Progress the Step counter   |
                \*---------------------------*/
                if (EffectStep < 30)
                {
                    EffectStep += 1;
                }
                else if (EffectStep >= 30)
                {
                    EffectStep = 1;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(OpenRGBEffectTab::FPS)); // 30 FPS
            }
            else std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }).detach();
}


/*--------------*\
| FPS Handling   |
\*--------------*/
void OpenRGBEffectTab::FPSSlider(int NewFPS)
{
    OpenRGBEffectTab::FPS = 1000/GetSpeed[NewFPS];
    ui->FPSCount->setText(QString().number(GetSpeed[NewFPS]));
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

    for (int EffectIndex = 0; EffectIndex < int(EffectList.size()); EffectIndex++)
    {
        RespectiveToPass[EffectIndex].clear();
    }

    /*--------------------------------------------------------*\
    | Grab new controllers and start making entries for them   |
    \*--------------------------------------------------------*/
    std::vector<RGBController*> NewControllers = ORGBPlugin::RMPointer->GetRGBControllers();
    for (int i = 0; i < int(NewControllers.size()); i++)
    {
        bool HasDirectMode = false;
        int  DirectID = 0;
        for (int ModeIndex = 0; ModeIndex < int(NewControllers[i]->modes.size()); ModeIndex++ )
        {
            if (NewControllers[i]->modes[ModeIndex].name == "Direct")
            {
                HasDirectMode = true;
                DirectID   = ModeIndex;
                break;
            }
        }

        BetterController NewItem;
        NewItem.Controller  = NewControllers[i];
        NewItem.Index       = i;
        NewItem.HasDirect   = HasDirectMode;
        NewItem.DirectIndex = DirectID;

        Controllers.push_back(NewItem);

        for (int EffectIndex = 0; EffectIndex < int(EffectList.size()); EffectIndex++)
        {
            OwnedControllerAndZones NewZoneMap;
            NewZoneMap.Controller = NewControllers[i];
            RespectiveToPass[EffectIndex].push_back(NewZoneMap);
        }

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
            DevIndex = DeviceID;
            break;
        }
        continue;
    }

    int DevTabIndex = DevIndex; // This is the index of the tab holding the device. It is mismatched because of the zone boxes
    DevIndex = DevIndex/2; // This is the actual index of the device

    // Grab the Device checkbox
    QCheckBox* DeviceSelected = qobject_cast<QCheckBox* >(ui->SelectDevices->cellWidget(DevTabIndex,1));
    if (Controllers[DevIndex].Controller->zones.size() > 1)
    {
        if ((DeviceSelected->isEnabled()) && (DeviceSelected->isChecked()))
        {
            // Make sure that the mode is direct
            if (Controllers[DevIndex].HasDirect && (Controllers[DevIndex].Controller->GetMode() != Controllers[DevIndex].DirectIndex))
            {
                Controllers[DevIndex].Controller->SetMode(Controllers[DevIndex].DirectIndex);
                Controllers[DevIndex].Controller->UpdateMode();
            }


            QTableWidget* ZoneTable = qobject_cast<QTableWidget*>(ui->SelectDevices->cellWidget( (DevTabIndex + 1), 0));
            /*-----------------------------------------*\
            | Add all non owned zones to the list       |
            \*-----------------------------------------*/
            for (int ZoneID = 0; ZoneID < int(Controllers[DevIndex].Controller->zones.size()); ZoneID++)
            {
                QCheckBox* ZoneSelected = qobject_cast<QCheckBox*>(ZoneTable->cellWidget(ZoneID,1));
                if (ZoneSelected->isEnabled() && !ZoneSelected->isChecked())
                {
                    ZoneOwnedBy Identifier;
                    Identifier.EffectName = EffectList[CurrentTab]->EffectDetails.EffectName;
                    Identifier.Zone = ZoneID;
                    Controllers[DevIndex].OwnedZones.push_back(Identifier);

                    RespectiveToPass[CurrentTab][DevIndex].OwnedZones.push_back(ZoneID);

                    ZoneSelected->setCheckState(Qt::Checked);
                }
            }
        }
        else if (DeviceSelected->isEnabled() && !DeviceSelected->isChecked())
        {
            QTableWidget* ZoneTable = qobject_cast<QTableWidget*>(ui->SelectDevices->cellWidget( (DevTabIndex + 1), 0));
            /*-----------------------------------------*\
            | Add all non owned zones to the list       |
            \*-----------------------------------------*/
            for (int ZoneID = 0; ZoneID < ZoneTable->rowCount(); ZoneID++)
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
                            for (int RMZone = 0; RMZone < int(RespectiveToPass[CurrentTab][DevIndex].OwnedZones.size()); RMZone++)
                            {
                                if (RespectiveToPass[CurrentTab][DevIndex].OwnedZones[RMZone] == ZoneID)
                                {
                                    RespectiveToPass[CurrentTab][DevIndex].OwnedZones.erase(RespectiveToPass[CurrentTab][DevIndex].OwnedZones.begin() + RMZone);
                                    break;
                                }
                            }
                            break;
                        }
                    }
                    // Set the state to unchecked
                    ZoneCheckBox->setCheckState(Qt::Unchecked);
                }
            }
        }
    }
    else
    {
        if (DeviceSelected->isEnabled() && DeviceSelected->isChecked())
        {
            ZoneOwnedBy ZO;
            ZO.EffectName = EffectList[CurrentTab]->EffectDetails.EffectName;
            ZO.Zone       = 0;
            Controllers[DevIndex].OwnedZones.push_back(ZO);
            RespectiveToPass[CurrentTab][DevIndex].OwnedZones.push_back(0);
        }
        else if (DeviceSelected->isEnabled() && !DeviceSelected->isChecked())
        {
            Controllers[DevIndex].OwnedZones.clear();
            RespectiveToPass[CurrentTab][DevIndex].OwnedZones.clear();
        }
    }
}

void OpenRGBEffectTab::ZoneSelectionChanged(QString DName)
{
    int DevIndex = 0;
    for (int DeviceID = 0; DeviceID < ui->SelectDevices->rowCount(); DeviceID++)
    {
        if (DeviceID%2) continue;
        int TempDevID = DeviceID/2;
        QTableWidgetItem* DeviceName = ui->SelectDevices->item(DeviceID,0);
        if ((DeviceName->text() + QString().number(Controllers[TempDevID].Index)) == DName)
        {
            DevIndex = DeviceID;
            break;
        }
        continue;
    }

    int DevTabIndex = DevIndex + 1; // This is the index of the table holding the device zones.
    DevIndex = DevIndex/2; // This is the actual index of the device

    // Grab the Zone table from the main device table
    QTableWidget* ZoneTable = qobject_cast<QTableWidget*>(ui->SelectDevices->cellWidget(DevTabIndex,0));

    bool CalledDirect = false;
    bool AllSelected = true;
    for (int ZoneID = 0; ZoneID < int(Controllers[DevIndex].Controller->zones.size()); ZoneID++)
    {
        // Grab the checkbox at the zone index
        QCheckBox* ZoneSelected = qobject_cast<QCheckBox*>(ZoneTable->cellWidget(ZoneID,1));

        if (ZoneSelected->isEnabled() && ZoneSelected->isChecked())
        {
            // Set direct if it is available
            if (!CalledDirect)
            {
                // Make sure that the device is in direct mode
                if (Controllers[DevIndex].HasDirect && (Controllers[DevIndex].Controller->GetMode() != Controllers[DevIndex].DirectIndex))
                {
                    Controllers[DevIndex].Controller->SetMode(Controllers[DevIndex].DirectIndex);
                    Controllers[DevIndex].Controller->UpdateMode();
                }
                CalledDirect = true;
            }

            bool AlreadyLocked = false;
            for (int OwnedZoneIndex = 0; OwnedZoneIndex < int(Controllers[DevIndex].OwnedZones.size()); OwnedZoneIndex++)
            {
                if (Controllers[DevIndex].OwnedZones[OwnedZoneIndex].Zone == ZoneID)
                {
                    AlreadyLocked = true;
                }
            }

            if (!AlreadyLocked)
            {
                //qDebug() << "Added Zone" << ZoneID << "to the list";

                // Create an identifier for the zone lock (Name and index)
                ZoneOwnedBy ZoneLock;
                ZoneLock.EffectName = EffectList[CurrentTab]->EffectDetails.EffectName;
                ZoneLock.Zone = ZoneID;

                Controllers[DevIndex].OwnedZones.push_back(ZoneLock);

                // Add this to the list of owned zones for a controller/effect
                RespectiveToPass[CurrentTab][DevIndex].OwnedZones.push_back(ZoneID);
            }
        }
        else if (ZoneSelected->isEnabled() && !ZoneSelected->isChecked())
        {
            AllSelected = false;
            for (int OwnedZoneIndex = 0; OwnedZoneIndex < int(Controllers[DevIndex].OwnedZones.size()); OwnedZoneIndex++)
            {
                if
                (
                     (Controllers[DevIndex].OwnedZones[OwnedZoneIndex].Zone == ZoneID) &&
                     Controllers[DevIndex].OwnedZones[OwnedZoneIndex].EffectName == EffectList[CurrentTab]->EffectDetails.EffectName
                )
                {
                    //qDebug() << "Removed Zone" << ZoneID << "From list";
                    Controllers[DevIndex].OwnedZones.erase(Controllers[DevIndex].OwnedZones.begin() + OwnedZoneIndex);

                    for (int RMZone = 0; RMZone < int(RespectiveToPass[CurrentTab][DevIndex].OwnedZones.size()); RMZone++)
                    {
                        if (RespectiveToPass[CurrentTab][DevIndex].OwnedZones[RMZone] == ZoneID)
                        {
                            // Remove this from the list of owned zones for a controller/effect
                            RespectiveToPass[CurrentTab][DevIndex].OwnedZones.erase(RespectiveToPass[CurrentTab][DevIndex].OwnedZones.begin() + RMZone);
                        }
                    }
                }
            }
        }
    }
    if (AllSelected)
    {
        int DevSelect = DevTabIndex - 1;
        QCheckBox* DevBox = qobject_cast<QCheckBox*>(ui->SelectDevices->cellWidget(DevSelect,1));
        DevBox->setCheckState(Qt::Checked);
    }
    else if (!AllSelected)
    {
        int DevSelect = DevTabIndex - 1;
        QCheckBox* DevBox = qobject_cast<QCheckBox*>(ui->SelectDevices->cellWidget(DevSelect,1));
        DevBox->setCheckState(Qt::Unchecked);
    }
}

void OpenRGBEffectTab::on_TabChange(int Tab)
{
    OpenRGBEffectTab::CurrentTab = Tab;
    for (int RowID = 0; RowID < ui->SelectDevices->rowCount()/2; RowID++)
    {
        bool AllSelected = true;
        bool AllLocked   = true;
        int ZoneBoxIndex = RowID * 2 + 1;
        if (Controllers[RowID].Controller->zones.size() > 1)
        {
            QTableWidget* ZoneTable = qobject_cast<QTableWidget*>(ui->SelectDevices->cellWidget(ZoneBoxIndex,0));

            for (int ZoneID = 0; ZoneID < ZoneTable->rowCount(); ZoneID++)
            {
                QCheckBox* ZoneSelected = qobject_cast<QCheckBox*>(ZoneTable->cellWidget(ZoneID,1));

                // If the controller isn't owned and it also isn't selected then set AllSelect to false
                if (!ZoneSelected->isChecked())
                {
                    AllSelected = false;
                }

                for (int CheckOwnedZones = 0; CheckOwnedZones < int(Controllers[RowID].OwnedZones.size()); CheckOwnedZones++)
                {
                    if  (Controllers[RowID].OwnedZones[CheckOwnedZones].Zone == ZoneID)
                    {
                        if (Controllers[RowID].OwnedZones[CheckOwnedZones].EffectName == EffectList[CurrentTab]->EffectDetails.EffectName)
                        {
                            ZoneSelected->setDisabled(false);
                            ZoneSelected->setToolTip("");
                        }
                        else
                        {
                            ZoneSelected->setDisabled(true);
                            ZoneSelected->setToolTip(QString().fromStdString("Owned by " + Controllers[RowID].OwnedZones[CheckOwnedZones].EffectName));
                        }
                    }
                }
                if (ZoneSelected->isEnabled())
                {
                    AllLocked = false;
                }
            }
            int DevSelect = ZoneBoxIndex - 1;
            QCheckBox* DeviceSelected = qobject_cast<QCheckBox*>(ui->SelectDevices->cellWidget(DevSelect,1));
            if (AllSelected)
            {
                DeviceSelected->setCheckState(Qt::Checked);
            }
            else if (!AllSelected)
            {
                DeviceSelected->setCheckState(Qt::Unchecked);
            }
            if (AllLocked)
            {
                DeviceSelected->setDisabled(true);
                DeviceSelected->setToolTip("No zones to select");
            }
            else if (!AllLocked)
            {
                DeviceSelected->setDisabled(false);
                DeviceSelected->setToolTip("");
            }
        }
        else
        {
            QCheckBox* DevBox = qobject_cast<QCheckBox*>(ui->SelectDevices->cellWidget(ZoneBoxIndex - 1,1));
            if (Controllers[RowID].OwnedZones.size() == 1)
            {
                if (Controllers[RowID].OwnedZones[0].EffectName == EffectList[CurrentTab]->EffectDetails.EffectName)
                {
                    DevBox->setEnabled(true);
                    DevBox->setToolTip("");
                }
                else
                {
                    DevBox->setEnabled(false);
                    DevBox->setToolTip(QString().fromStdString("Device Owned by " + Controllers[RowID].OwnedZones[0].EffectName));
                }
            }
            else
            {
                DevBox->setCheckState(Qt::Unchecked);
                DevBox->setToolTip("");
            }
        }
    }
}
