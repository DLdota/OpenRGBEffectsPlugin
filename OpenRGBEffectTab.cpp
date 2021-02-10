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
    OpenRGBEffectTab::EffectList.push_back(new SpectrumCycling);
    OpenRGBEffectTab::EffectList.push_back(new RainbowWave);
    OpenRGBEffectTab::EffectList.push_back(new StarryNight);
    OpenRGBEffectTab::EffectList.push_back(new GradientCycling);
    OpenRGBEffectTab::EffectList.push_back(new Breathing);
    OpenRGBEffectTab::EffectList.push_back(new Rain);
    OpenRGBEffectTab::EffectList.push_back(new Ambient);
    OpenRGBEffectTab::EffectList.push_back(new SeesawMotion);
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
        NewItem->setToolTip("This device doesn't have direct mode\nUsing an effect on a device WILL damage the flash or controller");
    }
    ui->SelectDevices->setItem(NewRow,0,NewItem);

    QCheckBox* SelectedBox = new QCheckBox();
    ui->SelectDevices->setCellWidget(NewRow,1,SelectedBox);

    // Map the button press to send a string with the device name and index to the function
    QSignalMapper* DeviceSelectionMapper = new QSignalMapper(SelectedBox);
    DeviceSelectionMapper->setMapping(SelectedBox, (QString().fromStdString(Controller->name) + QString().number(Index)) );

    connect(SelectedBox,SIGNAL(clicked()),DeviceSelectionMapper,SLOT(map()));
    connect(DeviceSelectionMapper,SIGNAL(mapped(QString)),this,SLOT( DeviceSelectionChanged(QString) ) );

    /*---------------------------------------------------*\
    | Map the reverse button if there is more than 1 zone |
    \*---------------------------------------------------*/
    QCheckBox* ReversedBox = new QCheckBox();
    ui->SelectDevices->setCellWidget(NewRow,2,ReversedBox);

    QSignalMapper* DeviceReversalMapper = new QSignalMapper(ReversedBox);
    DeviceReversalMapper->setMapping(ReversedBox, (QString().fromStdString(Controller->name) + QString().number(Index)) );

    connect(ReversedBox,SIGNAL(clicked()),DeviceReversalMapper,SLOT(map()));
    connect(DeviceReversalMapper,SIGNAL(mapped(QString)),this,SLOT( DeviceReversalChanged(QString) ) );

    /*----------------------*\
    | Create the Zone table  |
    \*----------------------*/
    QTableWidget* ZoneTableChecks = new QTableWidget;

    ZoneTableChecks->setColumnCount(3);

    ZoneTableChecks->setFixedWidth(320);

    ZoneTableChecks->setEditTriggers(QTableWidget::NoEditTriggers);
    ZoneTableChecks->setSelectionMode(QTableWidget::NoSelection);
    ZoneTableChecks->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ZoneTableChecks->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

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
            Controllers[Index].ReversedZones.push_back(false);
            ZoneTableChecks->setRowCount(ZoneTableChecks->rowCount() + 1);

            QTableWidgetItem* NewZoneName = new QTableWidgetItem(QString().fromStdString( ( "        " + Controller->zones[ZoneNum].name) ) ) ;

            if (!HasDirectMode)
            {
                NewZoneName->setForeground(Qt::red);
                NewZoneName->setToolTip("This device doesn't have direct mode\nnUsing an effect on a device WILL damage the flash or controller");
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
            connect(ZoneSelectionMapper,SIGNAL(mapped(QString)),this,SLOT( ZoneSelectionChanged(QString) ) );

            /*------------------------------------*\
            | Make mapping for reverse Checkbox    |
            \*------------------------------------*/
            QCheckBox* ZoneReversed = new QCheckBox();

            QSignalMapper* ZoneReversalMapper = new QSignalMapper(ZoneReversed);
            ZoneReversalMapper->setMapping(ZoneReversed, (QString().fromStdString(Controller->name) + QString().number(Index)) );

            connect(ZoneReversed,SIGNAL(clicked()),ZoneReversalMapper,SLOT(map()));
            connect(ZoneReversalMapper,SIGNAL(mapped(QString)),this,SLOT( ZoneReversalChanged(QString) ) );

            /*----------------*\
            | Add the widgets  |
            \*----------------*/
            ZoneTableChecks->setCellWidget(ZoneNum,1,ZoneSelected);
            ZoneTableChecks->setCellWidget(ZoneNum,2,ZoneReversed);
            RowHeight += 1;
        }
        ui->SelectDevices->setRowHeight((NewRow + 1),(31*RowHeight) );
    }
    else if (int(Controller->zones.size()) == 1)
    {
        Controllers[Index].ReversedZones.push_back(false);
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
    ui->SelectDevices->setMinimumWidth(332);
    ui->SelectDevices->setColumnCount(3);
    ui->SelectDevices->setHorizontalHeaderLabels({"Device","Enabled","Reversed"});

    ui->SelectDevices->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->SelectDevices->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    /*-------------------*\
    | Set collumn sizes   |
    \*-------------------*/
    std::vector<int> CollumnSizes = {167 , 75, 75};
    for (int i = 0; i < int(CollumnSizes.size()); i++)
    {
        ui->SelectDevices->setColumnWidth(i,CollumnSizes[i]);
    }

    /*------------------------------------------------*\
    | Register for callbacks and create effect thread  |
    \*------------------------------------------------*/
    ORGBPlugin::RMPointer->RegisterDeviceListChangeCallback(DeviceListChangedCallback, this);
    ORGBPlugin::RMPointer->RegisterDetectionProgressCallback(DeviceListChangedCallback, this);


    OpenRGBEffectTab::FPSDelay = 1000; // 1 second delay
    OpenRGBEffectTab::FPS      = 1;
    ui->FPSCount->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->FPSCount->setFixedWidth(20);
    ui->FPSCount->setText(QString().number(1));

    ui->FPSCount->adjustSize();

    ui->FPSSlider->setMaximum(int(OpenRGBEffectTab::GetSpeed.size()) - 1);
    ui->FPSSlider->setMinimum(0);
    connect(ui->FPSSlider,SIGNAL(valueChanged(int)),this,SLOT(FPSSlider(int)));

    /*----------------------------------*\
    | Create the effect handling thread  |
    \*----------------------------------*/
    StepEffectThread = new std::thread(&OpenRGBEffectTab::EffectStepTimer,this);

    DeviceListChanged();
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
        while (true) {
            if (int(OpenRGBEffectTab::ActiveEffects.size()) > 0)
            {
                auto start = std::chrono::steady_clock::now();

                for (int EffectIndex = 0; EffectIndex < int(OpenRGBEffectTab::ActiveEffects.size()); EffectIndex++)
                {
                    OpenRGBEffectTab::ActiveEffects[EffectIndex]->StepEffect(RespectiveToPass[OpenRGBEffectTab::ActiveEffects[EffectIndex]->EffectDetails.EffectIndex],OpenRGBEffectTab::FPS);
                }

                // After running through all of the effects proceed to set all of the LEDs on a per zone basis
                for (int ControllerID = 0; ControllerID < int(Controllers.size()); ControllerID++)
                {
                    if (Controllers[ControllerID].OwnedZones.size() > 0)
                    {
                        Controllers[ControllerID].Controller->UpdateLEDs();
                    }
                }

                auto end = std::chrono::steady_clock::now();

                int delta = OpenRGBEffectTab::FPSDelay - std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

                if(delta > 0)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(delta));
                }
                else
                {
                    // what could we really do more?
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
        }
    }).detach();
}


/*--------------*\
| FPS Handling   |
\*--------------*/
void OpenRGBEffectTab::FPSSlider(int NewFPS)
{
    OpenRGBEffectTab::FPSDelay = 1000/GetSpeed[NewFPS];
    OpenRGBEffectTab::FPS      = GetSpeed[NewFPS];

    ui->FPSCount->setText(QString().number(GetSpeed[NewFPS]));

    json PrevSettings = OpenRGBEffectTab::LoadPrevSetting();
    PrevSettings["FPS"] = GetSpeed[NewFPS];

    std::ofstream EffectFile((ORGBPlugin::RMPointer->GetConfigurationDirectory() + "/plugins/EffectSettings.json"), std::ios::out | std::ios::binary);
    if(EffectFile)
    {
        try{ EffectFile << PrevSettings.dump(4); }
        catch(std::exception e){}
        EffectFile.close();
    }
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
    if (ORGBPlugin::RMPointer->GetDetectionPercent() != 100) return;
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

    GivePreviousDevices();
}

/*-----------*\
| Settings    |
\*-----------*/
json OpenRGBEffectTab::LoadPrevSetting()
{
    json SettingsJson;

    /*---------------------------------------------------------*\
    | Open input file in binary mode                            |
    \*---------------------------------------------------------*/
    std::ifstream SFile(ORGBPlugin::RMPointer->GetConfigurationDirectory() + "/plugins/EffectSettings.json", std::ios::in | std::ios::binary);

    /*---------------------------------------------------------*\
    | Read settings into JSON store                             |
    \*---------------------------------------------------------*/
    if(SFile)
    {
        try
        {
            SFile >> SettingsJson;
            SFile.close();
        }
        catch(std::exception e){}
    }
    return SettingsJson;
}

void OpenRGBEffectTab::GivePreviousDevices()
{
    json UserSettings = LoadPrevSetting();

    if (UserSettings.contains("FPS"))
    {
        for (int FPSIdentifier = 0; FPSIdentifier < (int)GetSpeed.size(); FPSIdentifier++)
        {
            if (GetSpeed[FPSIdentifier] == UserSettings["FPS"])
            {
                ui->FPSSlider->setSliderPosition(FPSIdentifier);
                ui->FPSSlider->valueChanged(FPSIdentifier);
                break;
            }
        }
    }

    if (UserSettings.contains("Reversed"))
    {
        for (int DeviceIndex = 0; DeviceIndex < (int)UserSettings["Reversed"].size(); DeviceIndex++)
        {
            for (int ControllerID = 0; ControllerID < (int)ORGBPlugin::RMPointer->GetRGBControllers().size(); ControllerID++)
            {
                RGBController* Comp = Controllers[ControllerID].Controller;

                if
                (
                      ( UserSettings["Reversed"][DeviceIndex]["ControllerName"]        == Comp->name               )
                    &&( UserSettings["Reversed"][DeviceIndex]["ControllerDescription"] == Comp->description        )
                    &&( UserSettings["Reversed"][DeviceIndex]["ControllerLocation"]    == Comp->location           )
                    &&( UserSettings["Reversed"][DeviceIndex]["ControllerSerial"]      == Comp->serial             )
                    &&( UserSettings["Reversed"][DeviceIndex]["ControllerLEDCount"]    == Comp->colors.size()      )
                    &&( UserSettings["Reversed"][DeviceIndex]["ControllerZoneCount"]   == Comp->zones.size()       )
                )
                {
                    /*------------------------------------*\
                    | If the device has more than 1 zone   |
                    | Add zones on a per zone basis        |
                    \*------------------------------------*/
                    if (Comp->zones.size() > 1)
                    {
                        QTableWidget* ZoneReverseTable = qobject_cast<QTableWidget*>(ui->SelectDevices->cellWidget(( (ControllerID * 2) + 1),0));
                        for (int ZoneID = 0; ZoneID < (int)UserSettings["Reversed"][DeviceIndex]["ReversedZones"].size(); ZoneID++)
                        {
                            if (UserSettings["Reversed"][DeviceIndex]["ReversedZones"][ZoneID])
                            {
                                QCheckBox* ZoneBox = qobject_cast<QCheckBox*>(ZoneReverseTable->cellWidget(ZoneID,2));
                                ZoneBox->click();
                            }
                        }
                    }
                    else
                    {
                        if (UserSettings["Reversed"][DeviceIndex]["ReversedZones"][0])
                        {
                            QCheckBox* DeviceBox = qobject_cast<QCheckBox*>(ui->SelectDevices->cellWidget( (ControllerID*2), 2 ) );
                            DeviceBox->click();
                        }
                    }
                    break;
                }
            }
        }

    }

    if (UserSettings.contains("Effects"))
    {
        for (int EffectIndex = 0; EffectIndex < (int)EffectList.size(); EffectIndex++)
        {
            on_TabChange(EffectIndex);
            if (UserSettings["Effects"][EffectIndex].contains("EffectName"))
            {
                json DeviceList = UserSettings["Effects"][EffectIndex]["EffectSettings"]["Controllers"];

                for (int DeviceIndex = 0; DeviceIndex < (int)DeviceList.size(); DeviceIndex++)
                {
                    for (int ControllerID = 0; ControllerID < (int)Controllers.size(); ControllerID++)
                    {
                        RGBController* Comp = Controllers[ControllerID].Controller;

                        /*
                        qDebug() << QString().fromStdString(DeviceList[DeviceIndex]["ControllerName"]);
                        qDebug() << QString().fromStdString(Comp->name);

                        qDebug() << QString().fromStdString(DeviceList[DeviceIndex]["ControllerDescription"]);
                        qDebug() << QString().fromStdString(Comp->description);

                        qDebug() << QString().fromStdString(DeviceList[DeviceIndex]["ControllerLocation"]);
                        qDebug() << QString().fromStdString(Comp->location);

                        qDebug() << QString().fromStdString(DeviceList[DeviceIndex]["ControllerSerial"]);
                        qDebug() << QString().fromStdString(Comp->serial);

                        qDebug() << QString().number((int)DeviceList[DeviceIndex]["ControllerLEDCount"]);
                        qDebug() << QString().number(Comp->colors.size());
                        */

                        if
                        (
                              ( DeviceList[DeviceIndex]["ControllerName"]        == Comp->name               )
                            &&( DeviceList[DeviceIndex]["ControllerDescription"] == Comp->description        )
                            &&( DeviceList[DeviceIndex]["ControllerLocation"]    == Comp->location           )
                            &&( DeviceList[DeviceIndex]["ControllerSerial"]      == Comp->serial             )
                            &&( DeviceList[DeviceIndex]["ControllerLEDCount"]    == Comp->colors.size()      )
                        )
                        {
                            if (Comp->zones.size() > 1)
                            {
                                QTableWidget* ZoneSelectionTable = qobject_cast<QTableWidget*>(ui->SelectDevices->cellWidget(( (ControllerID * 2) + 1),0));
                                for (int ZoneID = 0; ZoneID < (int)DeviceList[DeviceIndex]["SelectedZones"].size(); ZoneID++)
                                {
                                    QCheckBox* ZoneBox = qobject_cast<QCheckBox*>(ZoneSelectionTable->cellWidget(DeviceList[DeviceIndex]["SelectedZones"][ZoneID],1));
                                    ZoneBox->click();
                                }
                            }
                            else
                            {
                                QCheckBox* DeviceBox = qobject_cast<QCheckBox*>(ui->SelectDevices->cellWidget( (ControllerID*2), 1 ) );
                                DeviceBox->click();
                            }
                            break;
                        }
                    }
                }
            }
        }
        on_TabChange(0);
    }
}

void OpenRGBEffectTab::SaveReversedSettings()
{
    json UserSettings = LoadPrevSetting();
    for (int DeviceIndex = 0; DeviceIndex < ui->SelectDevices->rowCount()/2; DeviceIndex++)
    {
        RGBController* Comp = Controllers[DeviceIndex].Controller;

        UserSettings["Reversed"][DeviceIndex]["ControllerName"]        = Comp->name;
        UserSettings["Reversed"][DeviceIndex]["ControllerDescription"] = Comp->description;
        UserSettings["Reversed"][DeviceIndex]["ControllerLocation"]    = Comp->location;
        UserSettings["Reversed"][DeviceIndex]["ControllerSerial"]      = Comp->serial;
        UserSettings["Reversed"][DeviceIndex]["ControllerLEDCount"]    = Comp->colors.size();
        UserSettings["Reversed"][DeviceIndex]["ControllerZoneCount"]   = Comp->zones.size();

        if (Controllers[DeviceIndex].Controller->zones.size() > 1)
        {
            QTableWidget* ZoneReversedTable = qobject_cast<QTableWidget*>(ui->SelectDevices->cellWidget( ((DeviceIndex*2)+1),0 ));
            for (int ZoneID = 0; ZoneID < ZoneReversedTable->rowCount(); ZoneID++)
            {
                QCheckBox* DevReversedBox = qobject_cast<QCheckBox*>(ZoneReversedTable->cellWidget(ZoneID , 2));
                if (DevReversedBox->isChecked())
                {
                    UserSettings["Reversed"][DeviceIndex]["ReversedZones"][ZoneID] = true;
                }
                else
                {
                    UserSettings["Reversed"][DeviceIndex]["ReversedZones"][ZoneID] = false;
                }
            }
        }
        else
        {
            QCheckBox* DevReversedBox = qobject_cast<QCheckBox*>(ui->SelectDevices->cellWidget(DeviceIndex*2 , 2));
            if (DevReversedBox->isChecked())
            {
                UserSettings["Reversed"][DeviceIndex]["ReversedZones"][0] = true;
            }
            else
            {
                UserSettings["Reversed"][DeviceIndex]["ReversedZones"][0] = false;
            }
        }
    }

    std::ofstream EffectFile((ORGBPlugin::RMPointer->GetConfigurationDirectory() + "/plugins/EffectSettings.json"), std::ios::out | std::ios::binary);
    if(EffectFile)
    {
        try{ EffectFile << UserSettings.dump(4); }
        catch(std::exception e){}
        EffectFile.close();
    }
}

/*-------------------------*\
| Tab / Selection Handling  |
\*-------------------------*/
void OpenRGBEffectTab::on_SelectAll_clicked()
{
    for (int DIndex = 0; DIndex < (int)Controllers.size(); DIndex++)
    {
        QCheckBox* DeviceSelected = qobject_cast<QCheckBox*>(ui->SelectDevices->cellWidget(DIndex*2,1));
        if (SelectsAll && DeviceSelected->isEnabled() && !DeviceSelected->isChecked())
        {
            DeviceSelected->click();
        }
        if (!SelectsAll && DeviceSelected->isEnabled() && DeviceSelected->isChecked())
        {
            DeviceSelected->click();
        }
    }
    SelectsAll = !SelectsAll;
    if (SelectsAll)
    {
        ui->SelectAll->setText("Select All");
    }
    else if (!SelectsAll)
    {
        ui->SelectAll->setText("Deselect All");
    }
}

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
            if (Controllers[DevIndex].HasDirect)
            {
                Controllers[DevIndex].Controller->SetMode(Controllers[DevIndex].DirectIndex);
                Controllers[DevIndex].Controller->UpdateMode();
            }
        }
        else if (DeviceSelected->isEnabled() && !DeviceSelected->isChecked())
        {
            Controllers[DevIndex].OwnedZones.clear();
            RespectiveToPass[CurrentTab][DevIndex].OwnedZones.clear();
        }
    }

    for (int EffectIndex = 0; EffectIndex < int(ActiveEffects.size()); EffectIndex++)
    {
        ActiveEffects[EffectIndex]->ASelectionWasChanged();
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

    for (int EffectIndex = 0; EffectIndex < int(ActiveEffects.size()); EffectIndex++)
    {
        ActiveEffects[EffectIndex]->ASelectionWasChanged();
    }
}

void OpenRGBEffectTab::DeviceReversalChanged(QString DName)
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
    QCheckBox* DeviceReversed = qobject_cast<QCheckBox* >(ui->SelectDevices->cellWidget(DevTabIndex,2));

    if ((DeviceReversed->isEnabled()) && (DeviceReversed->isChecked()))
    {
        if (Controllers[DevIndex].Controller->zones.size() == 1){Controllers[DevIndex].ReversedZones[0] = true; }
        else
        {
            QTableWidget* ZoneReverseTable = qobject_cast<QTableWidget*>(ui->SelectDevices->cellWidget((DevTabIndex + 1), 0));
            /*-----------------------------------------*\
            | Add all non owned zones to the list       |
            \*-----------------------------------------*/
            for (int ZoneID = 0; ZoneID < int(Controllers[DevIndex].Controller->zones.size()); ZoneID++)
            {
                QCheckBox* ZoneReversed = qobject_cast<QCheckBox*>(ZoneReverseTable->cellWidget(ZoneID,2));

                if (ZoneReversed->isEnabled() && !ZoneReversed->isChecked())
                {
                    Controllers[DevIndex].ReversedZones[ZoneID] = true;
                    ZoneReversed->setCheckState(Qt::Checked);
                }
            }
        }
    }
    else if (DeviceReversed->isEnabled() && !DeviceReversed->isChecked())
    {
        if (Controllers[DevIndex].Controller->zones.size() == 1){Controllers[DevIndex].ReversedZones[0] = false;}
        else
        {
            QTableWidget* ZoneReversalTable = qobject_cast<QTableWidget*>(ui->SelectDevices->cellWidget( (DevTabIndex + 1), 0));
            /*-----------------------------------------*\
            | Add all non reversed zones to the list    |
            \*-----------------------------------------*/
            for (int ZoneID = 0; ZoneID < ZoneReversalTable->rowCount(); ZoneID++)
            {
                QCheckBox* ZoneReverseBox = qobject_cast<QCheckBox*>(ZoneReversalTable->cellWidget(ZoneID,2));
                if (ZoneReverseBox->isEnabled() && ZoneReverseBox->isChecked())
                {
                    Controllers[DevIndex].ReversedZones[ZoneID] = false;
                    ZoneReverseBox->setCheckState(Qt::Unchecked);
                }
            }
        }
    }

    SaveReversedSettings();
}

void OpenRGBEffectTab::ZoneReversalChanged(QString DName)
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

    QTableWidget* ZoneTable = qobject_cast<QTableWidget*>(ui->SelectDevices->cellWidget( (DevTabIndex + 1) ,0) );

    bool AllSelected = true;
    for (int ZoneID = 0; ZoneID < int(Controllers[DevIndex].Controller->zones.size()); ZoneID++)
    {
        // Grab the checkbox at the zone index
        QCheckBox* ZoneReversed = qobject_cast<QCheckBox*>(ZoneTable->cellWidget(ZoneID,2));

        if (ZoneReversed->isEnabled() && ZoneReversed->isChecked())
        {
            Controllers[DevIndex].ReversedZones[ZoneID] = true;
        }
        else if (ZoneReversed->isEnabled() && !ZoneReversed->isChecked())
        {
            AllSelected = false;
            Controllers[DevIndex].ReversedZones[ZoneID] = false;
        }
    }

    if (AllSelected)
    {
        int DevSelect = DevTabIndex;
        QCheckBox* DevReversed = qobject_cast<QCheckBox*>(ui->SelectDevices->cellWidget(DevSelect,2));
        DevReversed->setCheckState(Qt::Checked);
    }
    else if (!AllSelected)
    {
        int DevSelect = DevTabIndex;
        QCheckBox* DevReversed = qobject_cast<QCheckBox*>(ui->SelectDevices->cellWidget(DevSelect,2));
        DevReversed->setCheckState(Qt::Unchecked);
    }

    SaveReversedSettings();
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

        QCheckBox* DeviceReversed = qobject_cast<QCheckBox*>(ui->SelectDevices->cellWidget( (RowID * 2) ,2));
        DeviceReversed->setEnabled(EffectList[CurrentTab]->EffectDetails.IsReversable);

        QTableWidget* ZoneTable = qobject_cast<QTableWidget*>(ui->SelectDevices->cellWidget( (RowID*2 + 1) ,0));
        for (int ZoneID = 0; ZoneID < ZoneTable->rowCount(); ZoneID++)
        {
            QCheckBox* ZoneReversed = qobject_cast<QCheckBox*>(ZoneTable->cellWidget(ZoneID,2));
            ZoneReversed->setEnabled(EffectList[CurrentTab]->EffectDetails.IsReversable);
        }
    }
}

/*-------------------------*\
| For the Effects to access |
\*-------------------------*/
bool OpenRGBEffectTab::CheckReversed(int DeviceIndex, int ZoneIndex)
{
    return Controllers[DeviceIndex].ReversedZones[ZoneIndex];
}

std::vector<OwnedControllerAndZones> OpenRGBEffectTab::GetToPass(int EffectIndex)
{
    return RespectiveToPass[EffectIndex];
}
