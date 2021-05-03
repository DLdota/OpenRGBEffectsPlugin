#include "OpenRGBEffectTab.h"
#include "OpenRGBEffectPage.h"
#include "EffectManager.h"

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

    QFrame* SelectionFrame = new QFrame();
    QHBoxLayout* SelectionLayout = new QHBoxLayout;
    SelectionLayout->setMargin(0);

    QCheckBox* SelectedBox = new QCheckBox(SelectionFrame);

    ResetButton* RButton = new ResetButton(Index,-1,SelectionFrame);
    connect(RButton,SIGNAL(Unlock(int,int)),this,SLOT(UnlockDevice(int,int)));

    SelectionLayout->addWidget(SelectedBox);
    SelectionLayout->addWidget(RButton);
    RButton->hide();

    SelectionLayout->setAlignment(SelectedBox,Qt::AlignHCenter);
    SelectionLayout->setAlignment(RButton,Qt::AlignHCenter);

    SelectionFrame->setLayout(SelectionLayout);
    ui->SelectDevices->setCellWidget(NewRow,1,SelectionFrame);

    // Map the button press to send a string with the device name and index to the function
    QSignalMapper* DeviceSelectionMapper = new QSignalMapper(SelectedBox);
    DeviceSelectionMapper->setMapping(SelectedBox, (QString().fromStdString(Controller->name) + QString().number(Index)) );

    connect(SelectedBox,SIGNAL(clicked()),DeviceSelectionMapper,SLOT(map()));
    connect(DeviceSelectionMapper,SIGNAL(mapped(QString)),this,SLOT( DeviceSelectionChanged(QString) ) );

    /*---------------------------------------------------*\
    | Map the reverse button if there is more than 1 zone |
    \*---------------------------------------------------*/
    QCheckBox* ReversedBox = new QCheckBox();
    SetStyleSheetMargins(ReversedBox);
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
    ZoneTableChecks->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    std::vector<int> CollumnSizes = {165 , 75, 75};
    for (unsigned int i = 0; i < CollumnSizes.size(); i++)
    {
        ZoneTableChecks->setColumnWidth(i,CollumnSizes[i]);
    }

    ZoneTableChecks->verticalHeader()->hide();
    ZoneTableChecks->horizontalHeader()->hide();

    if (Controller->zones.size() > 1)
    {
        int RowHeight = 0;
        for (unsigned int ZoneNum = 0; ZoneNum < Controller->zones.size(); ZoneNum++)
        {
            EffectManager::Get()->Controllers[Index].ReversedZones.push_back(false);
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
            QFrame* ZoneSelectionFrame = new QFrame();
            QHBoxLayout* ZoneSelectionLayout = new QHBoxLayout;

            QCheckBox* ZoneSelected = new QCheckBox(ZoneSelectionFrame);

            ResetButton* ZoneResetButton = new ResetButton(Index,ZoneNum,ZoneSelectionFrame);
            connect(ZoneResetButton,SIGNAL(Unlock(int,int)),this,SLOT(UnlockDevice(int,int)));

            ZoneSelectionLayout->addWidget(ZoneSelected);
            ZoneSelectionLayout->addWidget(ZoneResetButton);

            ZoneSelectionLayout->setAlignment(ZoneSelected,Qt::AlignHCenter);
            ZoneSelectionLayout->setAlignment(ZoneResetButton,Qt::AlignHCenter);
            ZoneResetButton->hide();

            ZoneSelectionLayout->setMargin(0);
            ZoneSelectionFrame->setLayout(ZoneSelectionLayout);

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
            ZoneTableChecks->setCellWidget(ZoneNum,1,ZoneSelectionFrame);
            ZoneTableChecks->setCellWidget(ZoneNum,2,ZoneReversed);

            SetStyleSheetMargins(ZoneReversed);

            RowHeight++;
        }

        ui->SelectDevices->setRowHeight((NewRow + 1),(31*RowHeight) );
    }
    else if (Controller->zones.size() == 1)
    {
        EffectManager::Get()->Controllers[Index].ReversedZones.push_back(false);
        ZoneTableChecks->setRowCount(0);
        ZoneTableChecks->setFixedHeight(0);
        ui->SelectDevices->setRowHeight((NewRow + 1),0);
    }

    ui->SelectDevices->setCellWidget((NewRow + 1),0,ZoneTableChecks);
}

void OpenRGBEffectTab::SetStyleSheetMargins(QCheckBox* CB)
{
    /*------------------------------------------------------------------------------------------------*\
    | While this function isn't strictly list entry creation it does Handling setting content margins  |
    \*------------------------------------------------------------------------------------------------*/
    CB->setStyleSheet("\
        margin-left:30%;  \
            margin-right:30%; \
    ");
}

QCheckBox* OpenRGBEffectTab::GetCheckboxFromFrame(QWidget* w)
{
    const QObjectList OList = w->children();
    QCheckBox* ReturnCheckbox = qobject_cast<QCheckBox*>(OList[0]);
    return ReturnCheckbox;
}

ResetButton* OpenRGBEffectTab::GetResetButtonFromFrame(QWidget* w)
{
    const QObjectList OList = w->children();
    ResetButton* ReturnPushButton = qobject_cast<ResetButton*>(OList[1]);
    return ReturnPushButton;
}

/*------------------------*\
| Contructor / Destructor  |
\*------------------------*/
OpenRGBEffectTab::OpenRGBEffectTab(QWidget *parent):
    QWidget(parent),
    ui(new Ui::OpenRGBEffectTab),
    Speeds({1,2,3,4,5,6,7,8,10,15,20,25,30,40,50,60})
{
    ui->setupUi(this);
    EffectManager::Get()->SetFPS(1);

    for (unsigned int i = 0; i < EffectManager::Get()->EffectList.size(); i++)
    {
        /*--------------------------------*\
        | Fill in the details              |
        \*--------------------------------*/
        EffectManager::Get()->EffectList[i]->EffectDetails = EffectManager::Get()->EffectList[i]->DefineEffectDetails();
        EffectManager::Get()->EffectList[i]->EffectDetails.EffectIndex = i;

        std::vector<OwnedControllerAndZones> BlankStarter;
        EffectManager::Get()->RespectiveToPass.push_back(BlankStarter);

        /*--------------------*\
        | Make the label       |
        \*--------------------*/
        QLabel* EffectTabLabel = new QLabel();
        EffectTabLabel->setText(QString().fromStdString(EffectManager::Get()->EffectList[i]->EffectDetails.EffectName));
        EffectTabLabel->setIndent(20);
        if(ORGBPlugin::DarkTheme)
        {
            EffectTabLabel->setGeometry(0, 25, 200, 50);
        }
        else
        {
            EffectTabLabel->setGeometry(0, 0, 200, 25);
        }
        OpenRGBEffectPage* EffectPage = new OpenRGBEffectPage(nullptr,EffectManager::Get()->EffectList[i]);
        ui->LeftTabBar->addTab(EffectPage,"");
        ui->LeftTabBar->tabBar()->setTabButton(ui->LeftTabBar->count() -1, QTabBar::LeftSide,EffectTabLabel);
    }

    connect(ui->LeftTabBar,SIGNAL(currentChanged(int)),this,SLOT(on_TabChanged(int)));
    CurrentTab = 0;

    /*-----------------------*\
    | Make the Device view    |
    \*-----------------------*/
    ui->SelectDevices->setColumnCount(3);
    ui->SelectDevices->setHorizontalHeaderLabels({"Device","Enabled","Reversed"});

    ui->SelectDevices->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->SelectDevices->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->SelectDevices->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    /*-------------------*\
    | Set collumn sizes   |
    \*-------------------*/
    std::vector<int> CollumnSizes = {167 , 75, 75};
    for (unsigned int i = 0; i < CollumnSizes.size(); i++)
    {
        ui->SelectDevices->setColumnWidth(i,CollumnSizes[i]);
    }

    /*------------------------------------------------*\
    | Register for callbacks and create effect thread  |
    \*------------------------------------------------*/
    ORGBPlugin::RMPointer->RegisterDeviceListChangeCallback(DeviceListChangedCallback, this);
    ORGBPlugin::RMPointer->RegisterDetectionProgressCallback(DeviceListChangedCallback, this);

    ui->FPSCount->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->FPSCount->setFixedWidth(20);
    ui->FPSCount->setText(QString().number(1));

    ui->FPSCount->adjustSize();

    ui->FPSSlider->setMaximum(Speeds.size() - 1);
    ui->FPSSlider->setMinimum(0);
    connect(ui->FPSSlider,SIGNAL(valueChanged(int)),this,SLOT(FPSSliderChanged(int)));

    /*-------------------------------------*\
    | Initial trigger (populates selectors) |
    \*-------------------------------------*/
    DeviceListChanged();
}

OpenRGBEffectTab::~OpenRGBEffectTab()
{
    delete ui;
}

/*--------------*\
| FPS Handling   |
\*--------------*/
void OpenRGBEffectTab::FPSSliderChanged(int NewFPS)
{    
    EffectManager::Get()->SetFPS(Speeds[NewFPS]);

    ui->FPSCount->setText(QString().number(Speeds[NewFPS]));

    json PrevSettings = OpenRGBEffectSettings::LoadUserSettings();
    PrevSettings["FPS"] = Speeds[NewFPS];

    OpenRGBEffectSettings::SaveUserSettings(PrevSettings);
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
    | Ignore incomplete detection   |
    \*-----------------------------*/
    if (ORGBPlugin::RMPointer->GetDetectionPercent() != 100)
    {
        return;
    }

    /*-----------------------------*\
    | Wipe the list of controllers  |
    \*-----------------------------*/
    EffectManager::Get()->ResetControllers();

    /*---------------------*\
    | Add Devices to list   |
    \*---------------------*/
    ui->SelectDevices->setRowCount(0);

    for (unsigned int i = 0; i < EffectManager::Get()->Controllers.size(); i++)
    {
        CreateDeviceSelection(EffectManager::Get()->Controllers[i].Controller, EffectManager::Get()->Controllers[i].Index, EffectManager::Get()->Controllers[i].HasDirect);
    }

    /*---------------------*\
    | Restore previous state|
    \*---------------------*/
    json UserSettings = OpenRGBEffectSettings::LoadUserSettings();

    if (UserSettings.contains("FPS"))
    {
        for (unsigned int FPSIdentifier = 0; FPSIdentifier < Speeds.size(); FPSIdentifier++)
        {
            if (Speeds[FPSIdentifier] == UserSettings["FPS"])
            {
                ui->FPSSlider->setSliderPosition(FPSIdentifier);
                EffectManager::Get()->SetFPS(Speeds[FPSIdentifier]);
                break;
            }
        }
    }

    if (UserSettings.contains("Reversed"))
    {
        for (unsigned int DeviceIndex = 0; DeviceIndex < UserSettings["Reversed"].size(); DeviceIndex++)
        {
            for (unsigned int ControllerID = 0; ControllerID < ORGBPlugin::RMPointer->GetRGBControllers().size(); ControllerID++)
            {
                RGBController* Comp = EffectManager::Get()->Controllers[ControllerID].Controller;

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
        for (int EffectIndex = 0; EffectIndex < (int)EffectManager::Get()->EffectList.size(); EffectIndex++)
        {
            on_TabChanged(EffectIndex);
            if (UserSettings["Effects"][EffectIndex].contains("EffectName"))
            {
                if (UserSettings["Effects"][EffectIndex]["EffectName"] != EffectManager::Get()->EffectList[EffectIndex]->EffectDetails.EffectName) continue;
                json DeviceList = UserSettings["Effects"][EffectIndex]["EffectSettings"]["Controllers"];

                for (int DeviceIndex = 0; DeviceIndex < (int)DeviceList.size(); DeviceIndex++)
                {
                    for (int ControllerID = 0; ControllerID < (int)EffectManager::Get()->Controllers.size(); ControllerID++)
                    {
                        RGBController* Comp = EffectManager::Get()->Controllers[ControllerID].Controller;

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
                                for (unsigned int ZoneID = 0; ZoneID < DeviceList[DeviceIndex]["SelectedZones"].size(); ZoneID++)
                                {
                                    if (DeviceList[DeviceIndex]["SelectedZones"][ZoneID] < Comp->zones.size())
                                    {
                                        QCheckBox* ZoneBox = GetCheckboxFromFrame(ZoneSelectionTable->cellWidget(DeviceList[DeviceIndex]["SelectedZones"][ZoneID],1));// qobject_cast<QCheckBox*>;
                                        ZoneBox->click();
                                    }
                                }
                            }
                            else
                            {
                                QCheckBox* DeviceBox = GetCheckboxFromFrame(ui->SelectDevices->cellWidget( (ControllerID*2), 1 ));
                                DeviceBox->click();
                            }
                            break;
                        }
                    }
                }
            }
        }
        on_TabChanged(0);
    }
}

void OpenRGBEffectTab::SaveReversedSettings()
{
    json UserSettings = OpenRGBEffectSettings::LoadUserSettings();
    for (int DeviceIndex = 0; DeviceIndex < ui->SelectDevices->rowCount()/2; DeviceIndex++)
    {
        RGBController* Comp = EffectManager::Get()->Controllers[DeviceIndex].Controller;

        UserSettings["Reversed"][DeviceIndex]["ControllerName"]        = Comp->name;
        UserSettings["Reversed"][DeviceIndex]["ControllerDescription"] = Comp->description;
        UserSettings["Reversed"][DeviceIndex]["ControllerLocation"]    = Comp->location;
        UserSettings["Reversed"][DeviceIndex]["ControllerSerial"]      = Comp->serial;
        UserSettings["Reversed"][DeviceIndex]["ControllerLEDCount"]    = Comp->colors.size();
        UserSettings["Reversed"][DeviceIndex]["ControllerZoneCount"]   = Comp->zones.size();

        if (EffectManager::Get()->Controllers[DeviceIndex].Controller->zones.size() > 1)
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

    OpenRGBEffectSettings::SaveUserSettings(UserSettings);
}

/*-------------------------*\
| Tab / Selection Handling  |
\*-------------------------*/
void OpenRGBEffectTab::on_SelectAll_clicked()
{
    for (unsigned int DIndex = 0; DIndex < EffectManager::Get()->Controllers.size(); DIndex++)
    {
        QCheckBox* DeviceSelected = GetCheckboxFromFrame(ui->SelectDevices->cellWidget(DIndex*2,1));
        if (SelectsAll && DeviceSelected->isEnabled() && !DeviceSelected->isChecked())
        {
            DeviceSelected->click();
        }

        if (!SelectsAll && DeviceSelected->isEnabled() && DeviceSelected->isChecked())
        {
            DeviceSelected->click();
        }
        else if (!SelectsAll && !DeviceSelected->isEnabled() && DeviceSelected->isChecked())
        {
            ResetButton* RButton = GetResetButtonFromFrame(ui->SelectDevices->cellWidget(DIndex*2,1));
            RButton->click();
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
        if ((DeviceName->text() + QString().number(EffectManager::Get()->Controllers[TempDevID].Index)) == DName)
        {
            DevIndex = DeviceID;
            break;
        }
        continue;
    }

    int DevTabIndex = DevIndex; // This is the index of the tab holding the device. It is mismatched because of the zone boxes
    DevIndex = DevIndex/2; // This is the actual index of the device

    // Grab the Device checkbox
    QCheckBox* DeviceSelected = GetCheckboxFromFrame(ui->SelectDevices->cellWidget(DevTabIndex,1));
    if (EffectManager::Get()->Controllers[DevIndex].Controller->zones.size() > 1)
    {
        if ((DeviceSelected->isEnabled()) && (DeviceSelected->isChecked()))
        {
            // Make sure that the mode is directOpenRGBEffectTab::
            if (EffectManager::Get()->Controllers[DevIndex].HasDirect && (EffectManager::Get()->Controllers[DevIndex].Controller->GetMode() != EffectManager::Get()->Controllers[DevIndex].DirectIndex))
            {
                EffectManager::Get()->Controllers[DevIndex].Controller->SetMode(EffectManager::Get()->Controllers[DevIndex].DirectIndex);
                EffectManager::Get()->Controllers[DevIndex].Controller->UpdateMode();
            }

            QTableWidget* ZoneTable = qobject_cast<QTableWidget*>(ui->SelectDevices->cellWidget( (DevTabIndex + 1), 0));
            /*-----------------------------------------*\
            | Add all non owned zones to the list       |
            \*-----------------------------------------*/
            for (unsigned int ZoneID = 0; ZoneID < EffectManager::Get()->Controllers[DevIndex].Controller->zones.size(); ZoneID++)
            {
                QCheckBox* ZoneSelected = GetCheckboxFromFrame(ZoneTable->cellWidget(ZoneID,1));
                if (ZoneSelected->isEnabled() && !ZoneSelected->isChecked())
                {
                    ZoneOwnedBy Identifier;
                    Identifier.EffectName = EffectManager::Get()->EffectList[CurrentTab]->EffectDetails.EffectName;
                    Identifier.Zone = ZoneID;
                    Identifier.EffectIndex = CurrentTab;

                    EffectManager::Get()->Controllers[DevIndex].OwnedZones.push_back(Identifier);
                    EffectManager::Get()->RespectiveToPass[CurrentTab][DevIndex].OwnedZones.push_back(ZoneID);

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
            for (unsigned int ZoneID = 0; ZoneID < (unsigned int)ZoneTable->rowCount(); ZoneID++)
            {
                QCheckBox* ZoneCheckBox = GetCheckboxFromFrame(ZoneTable->cellWidget(ZoneID,1));
                if (ZoneCheckBox->isEnabled() && ZoneCheckBox->isChecked())
                {
                    // Create this regardless so that we have something more specific to check against
                    ZoneOwnedBy Identifier;
                    Identifier.EffectName  = EffectManager::Get()->EffectList[CurrentTab]->EffectDetails.EffectName;
                    Identifier.Zone        = ZoneID;
                    Identifier.EffectIndex = CurrentTab;

                    for (unsigned int OwnedZoneIndex = 0; OwnedZoneIndex < EffectManager::Get()->Controllers[DevIndex].OwnedZones.size(); OwnedZoneIndex++)
                    {
                        // This is just to have a shorter variable name
                        ZoneOwnedBy CheckAgainst = EffectManager::Get()->Controllers[DevIndex].OwnedZones[OwnedZoneIndex];
                        if ((CheckAgainst.EffectName == Identifier.EffectName) && (CheckAgainst.Zone == Identifier.Zone))
                        {
                            EffectManager::Get()->Controllers[DevIndex].OwnedZones.erase(EffectManager::Get()->Controllers[DevIndex].OwnedZones.begin() + OwnedZoneIndex);
                            for (unsigned int RMZone = 0; RMZone < EffectManager::Get()->RespectiveToPass[CurrentTab][DevIndex].OwnedZones.size(); RMZone++)
                            {
                                if (EffectManager::Get()->RespectiveToPass[CurrentTab][DevIndex].OwnedZones[RMZone] == ZoneID)
                                {
                                    EffectManager::Get()->RespectiveToPass[CurrentTab][DevIndex].OwnedZones.erase(EffectManager::Get()->RespectiveToPass[CurrentTab][DevIndex].OwnedZones.begin() + RMZone);
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
            ZO.EffectName  = EffectManager::Get()->EffectList[CurrentTab]->EffectDetails.EffectName;
            ZO.Zone        = 0;
            ZO.EffectIndex = CurrentTab;
            EffectManager::Get()->Controllers[DevIndex].OwnedZones.push_back(ZO);
            EffectManager::Get()->RespectiveToPass[CurrentTab][DevIndex].OwnedZones.push_back(0);
            if (EffectManager::Get()->Controllers[DevIndex].HasDirect)
            {
                EffectManager::Get()->Controllers[DevIndex].Controller->SetMode(EffectManager::Get()->Controllers[DevIndex].DirectIndex);
                EffectManager::Get()->Controllers[DevIndex].Controller->UpdateMode();
            }
        }
        else if (DeviceSelected->isEnabled() && !DeviceSelected->isChecked())
        {
            EffectManager::Get()->Controllers[DevIndex].OwnedZones.clear();
            EffectManager::Get()->RespectiveToPass[CurrentTab][DevIndex].OwnedZones.clear();
        }
    }

    EffectManager::Get()->EffectList[CurrentTab]->ASelectionWasChanged(EffectManager::Get()->RespectiveToPass[EffectManager::Get()->EffectList[CurrentTab]->EffectDetails.EffectIndex]);
}

void OpenRGBEffectTab::ZoneSelectionChanged(QString DName)
{
    int DevIndex = 0;
    for (int DeviceID = 0; DeviceID < ui->SelectDevices->rowCount(); DeviceID++)
    {
        if (DeviceID%2) continue;
        int TempDevID = DeviceID/2;
        QTableWidgetItem* DeviceName = ui->SelectDevices->item(DeviceID,0);
        if ((DeviceName->text() + QString().number(EffectManager::Get()->Controllers[TempDevID].Index)) == DName)
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
    for (unsigned int ZoneID = 0; ZoneID < EffectManager::Get()->Controllers[DevIndex].Controller->zones.size(); ZoneID++)
    {
        // Grab the checkbox at the zone index
        QCheckBox* ZoneSelected = GetCheckboxFromFrame(ZoneTable->cellWidget(ZoneID,1));

        if (ZoneSelected->isEnabled() && ZoneSelected->isChecked())
        {
            // Set direct if it is available
            if (!CalledDirect)
            {
                // Make sure that the device is in direct mode
                if (EffectManager::Get()->Controllers[DevIndex].HasDirect && (EffectManager::Get()->Controllers[DevIndex].Controller->GetMode() != EffectManager::Get()->Controllers[DevIndex].DirectIndex))
                {
                    EffectManager::Get()->Controllers[DevIndex].Controller->SetMode(EffectManager::Get()->Controllers[DevIndex].DirectIndex);
                    EffectManager::Get()->Controllers[DevIndex].Controller->UpdateMode();
                }
                CalledDirect = true;
            }

            bool AlreadyLocked = false;
            for (unsigned int OwnedZoneIndex = 0; OwnedZoneIndex < EffectManager::Get()->Controllers[DevIndex].OwnedZones.size(); OwnedZoneIndex++)
            {
                if (EffectManager::Get()->Controllers[DevIndex].OwnedZones[OwnedZoneIndex].Zone == ZoneID)
                {
                    AlreadyLocked = true;
                }
            }

            if (!AlreadyLocked)
            {
                //qDebug() << "Added Zone" << ZoneID << "to the list";

                // Create an identifier for the zone lock (Name and index)
                ZoneOwnedBy ZoneLock;
                ZoneLock.EffectName = EffectManager::Get()->EffectList[CurrentTab]->EffectDetails.EffectName;
                ZoneLock.Zone = ZoneID;
                ZoneLock.EffectIndex = CurrentTab;

                EffectManager::Get()->Controllers[DevIndex].OwnedZones.push_back(ZoneLock);

                // Add this to the list of owned zones for a controller/effect
                EffectManager::Get()->RespectiveToPass[CurrentTab][DevIndex].OwnedZones.push_back(ZoneID);
            }
        }
        else if (ZoneSelected->isEnabled() && !ZoneSelected->isChecked())
        {
            AllSelected = false;
            for (unsigned int OwnedZoneIndex = 0; OwnedZoneIndex < EffectManager::Get()->Controllers[DevIndex].OwnedZones.size(); OwnedZoneIndex++)
            {
                if
                        (
                         (EffectManager::Get()->Controllers[DevIndex].OwnedZones[OwnedZoneIndex].Zone == ZoneID) &&
                         EffectManager::Get()->Controllers[DevIndex].OwnedZones[OwnedZoneIndex].EffectName == EffectManager::Get()->EffectList[CurrentTab]->EffectDetails.EffectName
                         )
                {
                    //qDebug() << "Removed Zone" << ZoneID << "From list";
                    EffectManager::Get()->Controllers[DevIndex].OwnedZones.erase(EffectManager::Get()->EffectManager::Get()->Controllers[DevIndex].OwnedZones.begin() + OwnedZoneIndex);

                    for (unsigned int RMZone = 0; RMZone < EffectManager::Get()->RespectiveToPass[CurrentTab][DevIndex].OwnedZones.size(); RMZone++)
                    {
                        if (EffectManager::Get()->RespectiveToPass[CurrentTab][DevIndex].OwnedZones[RMZone] == ZoneID)
                        {
                            // Remove this from the list of owned zones for a controller/effect
                            EffectManager::Get()->RespectiveToPass[CurrentTab][DevIndex].OwnedZones.erase(EffectManager::Get()->RespectiveToPass[CurrentTab][DevIndex].OwnedZones.begin() + RMZone);
                        }
                    }
                }
            }
        }
    }
    if (AllSelected)
    {
        int DevSelect = DevTabIndex - 1;
        QCheckBox* DevBox = GetCheckboxFromFrame(ui->SelectDevices->cellWidget(DevSelect,1));
        DevBox->setCheckState(Qt::Checked);
    }
    else if (!AllSelected)
    {
        int DevSelect = DevTabIndex - 1;
        QCheckBox* DevBox = GetCheckboxFromFrame(ui->SelectDevices->cellWidget(DevSelect,1));
        DevBox->setCheckState(Qt::Unchecked);
    }

    EffectManager::Get()->EffectList[CurrentTab]->ASelectionWasChanged(EffectManager::Get()->RespectiveToPass[EffectManager::Get()->EffectList[CurrentTab]->EffectDetails.EffectIndex]);
}

void OpenRGBEffectTab::DeviceReversalChanged(QString DName)
{
    int DevIndex = 0;
    for (int DeviceID = 0; DeviceID < ui->SelectDevices->rowCount(); DeviceID++)
    {
        if (DeviceID%2) continue;
        int TempDevID = DeviceID/2;
        QTableWidgetItem* DeviceName = ui->SelectDevices->item(DeviceID,0);
        if ((DeviceName->text() + QString().number(EffectManager::Get()->Controllers[TempDevID].Index)) == DName)
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
        if (EffectManager::Get()->Controllers[DevIndex].Controller->zones.size() == 1){EffectManager::Get()->Controllers[DevIndex].ReversedZones[0] = true; }
        else
        {
            QTableWidget* ZoneReverseTable = qobject_cast<QTableWidget*>(ui->SelectDevices->cellWidget((DevTabIndex + 1), 0));
            /*-----------------------------------------*\
            | Add all non owned zones to the list       |
            \*-----------------------------------------*/
            for (unsigned int ZoneID = 0; ZoneID < EffectManager::Get()->Controllers[DevIndex].Controller->zones.size(); ZoneID++)
            {
                QCheckBox* ZoneReversed = qobject_cast<QCheckBox*>(ZoneReverseTable->cellWidget(ZoneID,2));

                if (ZoneReversed->isEnabled() && !ZoneReversed->isChecked())
                {
                    EffectManager::Get()->Controllers[DevIndex].ReversedZones[ZoneID] = true;
                    ZoneReversed->setCheckState(Qt::Checked);
                }
            }
        }
    }
    else if (DeviceReversed->isEnabled() && !DeviceReversed->isChecked())
    {
        if (EffectManager::Get()->Controllers[DevIndex].Controller->zones.size() == 1){EffectManager::Get()->Controllers[DevIndex].ReversedZones[0] = false;}
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
                    EffectManager::Get()->Controllers[DevIndex].ReversedZones[ZoneID] = false;
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
        if ((DeviceName->text() + QString().number(EffectManager::Get()->Controllers[TempDevID].Index)) == DName)
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
    for (unsigned int ZoneID = 0; ZoneID < EffectManager::Get()->Controllers[DevIndex].Controller->zones.size(); ZoneID++)
    {
        // Grab the checkbox at the zone index
        QCheckBox* ZoneReversed = qobject_cast<QCheckBox*>(ZoneTable->cellWidget(ZoneID,2));

        if (ZoneReversed->isEnabled() && ZoneReversed->isChecked())
        {
            EffectManager::Get()->Controllers[DevIndex].ReversedZones[ZoneID] = true;
        }
        else if (ZoneReversed->isEnabled() && !ZoneReversed->isChecked())
        {
            AllSelected = false;
            EffectManager::Get()->Controllers[DevIndex].ReversedZones[ZoneID] = false;
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

void OpenRGBEffectTab::UnlockDevice(int Device, int Zone)
{
    // -1 is the entire device
    if (Zone == -1)
    {
        if (EffectManager::Get()->Controllers[Device].Controller->zones.size() > 1)
        {
            for (unsigned int ZoneID = 0; ZoneID < EffectManager::Get()->Controllers[Device].Controller->zones.size(); ZoneID++)
            {
                QTableWidget* ZoneTable = qobject_cast<QTableWidget*>(ui->SelectDevices->cellWidget(((Device * 2) + 1),0));

                for (int ZID = 0; ZID < ZoneTable->rowCount(); ZID++)
                {
                    QCheckBox* ZoneSelected = GetCheckboxFromFrame(ZoneTable->cellWidget(ZID,1));

                    if (!ZoneSelected->isEnabled())
                    {
                        UnlockDevice(Device,ZID);
                    }
                }
            }
        }
        else
        {
            int EffectIndex = EffectManager::Get()->Controllers[Device].OwnedZones[0].EffectIndex;
            OwnedControllerAndZones ZONES = EffectManager::Get()->RespectiveToPass[EffectIndex][Device];
            EffectManager::Get()->RespectiveToPass[EffectIndex][Device].OwnedZones.clear();
            EffectManager::Get()->Controllers[Device].OwnedZones.clear();
        }

        QCheckBox* SelectDevice = GetCheckboxFromFrame(ui->SelectDevices->cellWidget((Device * 2),1));
        ResetButton* ResetDevice = GetResetButtonFromFrame(ui->SelectDevices->cellWidget((Device * 2),1));
        SelectDevice->setDisabled(false);
        SelectDevice->setChecked(false);
        SelectDevice->show();
        ResetDevice->hide();
    }
    else
    {
        for (unsigned int ZoneID = 0; ZoneID < EffectManager::Get()->Controllers[Device].OwnedZones.size(); ZoneID++)
        {
            if (EffectManager::Get()->Controllers[Device].OwnedZones[ZoneID].Zone == (unsigned int) Zone)
            {
                int EffectIndex = EffectManager::Get()->Controllers[Device].OwnedZones[ZoneID].EffectIndex;
                OwnedControllerAndZones ZONES = EffectManager::Get()->RespectiveToPass[EffectIndex][Device];
                for (unsigned int SubZoneID = 0; SubZoneID < ZONES.OwnedZones.size(); SubZoneID++)
                {
                    if (ZONES.OwnedZones[SubZoneID] == (unsigned int) Zone)
                    {
                        EffectManager::Get()->RespectiveToPass[EffectIndex][Device].OwnedZones.erase(EffectManager::Get()->RespectiveToPass[EffectIndex][Device].OwnedZones.begin() + SubZoneID);
                        break;
                    }
                }
                EffectManager::Get()->Controllers[Device].OwnedZones.erase(EffectManager::Get()->Controllers[Device].OwnedZones.begin() + ZoneID);
                break;
            }
        }
        QTableWidget* ZoneTable = qobject_cast<QTableWidget*>(ui->SelectDevices->cellWidget(((Device * 2) + 1),0));
        QCheckBox* ZoneSelected = GetCheckboxFromFrame(ZoneTable->cellWidget(Zone,1));
        ResetButton* ResetZone = GetResetButtonFromFrame(ZoneTable->cellWidget(Zone,1));

        ZoneSelected->setDisabled(false);
        ZoneSelected->setChecked(false);
        ZoneSelected->show();
        ResetZone->hide();
    }

    on_TabChanged(CurrentTab);
}

void OpenRGBEffectTab::on_TabChanged(int Tab)
{
    bool FullSelectAll = true;
    OpenRGBEffectTab::CurrentTab = Tab;

    for (int RowID = 0; RowID < ui->SelectDevices->rowCount()/2; RowID++)
    {
        bool AllSelected = true;
        bool AllLocked   = true;
        int ZoneBoxIndex = RowID * 2 + 1;
        if (EffectManager::Get()->Controllers[RowID].Controller->zones.size() > 1)
        {
            /*----------------------*\
            | Get the list of zones  |
            \*----------------------*/
            QTableWidget* ZoneTable = qobject_cast<QTableWidget*>(ui->SelectDevices->cellWidget(ZoneBoxIndex,0));

            for (unsigned int ZoneID = 0; ZoneID < (unsigned int) ZoneTable->rowCount(); ZoneID++)
            {
                QCheckBox* ZoneSelected = GetCheckboxFromFrame(ZoneTable->cellWidget(ZoneID,1));
                ResetButton* ResetZone = GetResetButtonFromFrame(ZoneTable->cellWidget(ZoneID,1));
                /*-------------------------------------------------------------------------------------*\
                | If the controller isn't owned and it also isn't selected then set AllSelect to false  |
                \*-------------------------------------------------------------------------------------*/
                if (!ZoneSelected->isChecked())
                {
                    AllSelected = false;
                }

                for (unsigned int CheckOwnedZones = 0; CheckOwnedZones < EffectManager::Get()->Controllers[RowID].OwnedZones.size(); CheckOwnedZones++)
                {
                    if  (EffectManager::Get()->Controllers[RowID].OwnedZones[CheckOwnedZones].Zone == ZoneID)
                    {
                        if (EffectManager::Get()->Controllers[RowID].OwnedZones[CheckOwnedZones].EffectName == EffectManager::Get()->EffectList[CurrentTab]->EffectDetails.EffectName)
                        {
                            /*------------------------------------------------------*\
                            | This zone is owned by this effect so we can unlock it  |
                            \*------------------------------------------------------*/
                            ResetZone->hide();
                            ZoneSelected->setDisabled(false);
                            ZoneSelected->setToolTip("");
                            ZoneSelected->show();
                        }
                        else
                        {
                            /*-------------------------------------------------------------------*\
                            | This zone is owned by a different effect so disable clicking on it  |
                            \*-------------------------------------------------------------------*/
                            ResetZone->show();
                            ZoneSelected->setDisabled(true);
                            ZoneSelected->setToolTip(QString().fromStdString("Owned by " + EffectManager::Get()->Controllers[RowID].OwnedZones[CheckOwnedZones].EffectName));
                            ZoneSelected->hide();
                        }
                    }
                }
                /*--------------------------------------------------------------*\
                | If the button is enabled then not all of the zones are locked  |
                \*--------------------------------------------------------------*/
                if (ZoneSelected->isEnabled())
                {
                    AllLocked = false;
                }
            }
            int DevSelect = ZoneBoxIndex - 1;
            QCheckBox* DeviceSelected = GetCheckboxFromFrame(ui->SelectDevices->cellWidget(DevSelect,1));
            ResetButton* ResetDevice = GetResetButtonFromFrame(ui->SelectDevices->cellWidget(DevSelect,1));

            DeviceSelected->setCheckState(AllSelected ? Qt::Checked : Qt::Unchecked);

            if (AllLocked)
            {
                DeviceSelected->setDisabled(true);
                DeviceSelected->hide();
                DeviceSelected->setToolTip("No zones to select");
                ResetDevice->show();
            }
            else if (!AllLocked)
            {
                ResetDevice->hide();
                DeviceSelected->setDisabled(false);
                DeviceSelected->setToolTip("");
                DeviceSelected->show();
            }
        }
        /*-----------------------------*\
        | If the zone has less 2 zones  |
        \*-----------------------------*/
        else
        {
            QCheckBox* DevBox = GetCheckboxFromFrame(ui->SelectDevices->cellWidget(ZoneBoxIndex - 1,1));
            ResetButton* ResetDev = GetResetButtonFromFrame(ui->SelectDevices->cellWidget(ZoneBoxIndex - 1,1));
            /*-----------------------*\
            | If the device is owned  |
            \*-----------------------*/
            if (EffectManager::Get()->Controllers[RowID].OwnedZones.size() == 1)
            {
                /*------------------------------------------------------------------------------*\
                | If the current effect name is equal to the effect that the device is owned by  |
                \*------------------------------------------------------------------------------*/
                if (EffectManager::Get()->Controllers[RowID].OwnedZones[0].EffectName == EffectManager::Get()->EffectList[CurrentTab]->EffectDetails.EffectName)
                {
                    /*-------------------------------------------------------------*\
                    | Allow unchecking/unchecking since it is owned by this effect  |
                    \*-------------------------------------------------------------*/
                    DevBox->setDisabled(false);
                    DevBox->setToolTip("");
                    DevBox->show();
                    ResetDev->hide();
                }
                else
                {
                    /*---------------------------------------------------------------*\
                    | This device is owned by a different effect so should be locked  |
                    \*---------------------------------------------------------------*/
                    DevBox->setDisabled(true);
                    DevBox->setToolTip(QString().fromStdString("Device Owned by " + EffectManager::Get()->Controllers[RowID].OwnedZones[0].EffectName));
                    DevBox->hide();
                    ResetDev->show();
                }
            }
            else
            {
                /*---------------------------------------------------------------*\
                | If the device isn't owned the declare it as such and unlock it  |
                \*---------------------------------------------------------------*/
                DevBox->setCheckState(Qt::Unchecked);
                DevBox->setToolTip("");
            }
        }

        /*---------------------------------------------------*\
        | Set reverse checkbox state to enabled or disabled   |
        | depending on whether or not the effect supports it  |
        \*---------------------------------------------------*/

        if (!AllSelected)
        {
            FullSelectAll = false;
        }

        QCheckBox* DeviceReversed = qobject_cast<QCheckBox*>(ui->SelectDevices->cellWidget( (RowID * 2) ,2));
        DeviceReversed->setEnabled(EffectManager::Get()->EffectList[CurrentTab]->EffectDetails.IsReversable);

        /*----------------------------------------------*\
        | Set all of the zones reversal options as well  |
        \*----------------------------------------------*/
        QTableWidget* ZoneTable = qobject_cast<QTableWidget*>(ui->SelectDevices->cellWidget( (RowID*2 + 1) ,0));
        for (int ZoneID = 0; ZoneID < ZoneTable->rowCount(); ZoneID++)
        {
            QCheckBox* ZoneReversed = qobject_cast<QCheckBox*>(ZoneTable->cellWidget(ZoneID,2));
            ZoneReversed->setEnabled(EffectManager::Get()->EffectList[CurrentTab]->EffectDetails.IsReversable);
        }
    }

    SelectsAll = !FullSelectAll;
    if (SelectsAll)
    {
        ui->SelectAll->setText("Select All");
    }
    else if (!SelectsAll)
    {
        ui->SelectAll->setText("Deselect All");
    }
}

