#include "OpenRGBEffectPage.h"
#include "ColorWheel.h"

/*-----------------------*\
| Constructor/Destructor  |
\*-----------------------*/
OpenRGBEffectPage::OpenRGBEffectPage(QWidget *parent, RGBEffect* EFCT): QWidget(parent), ui(new Ui::OpenRGBEffectPage)
{
    ui->setupUi(this);

    OpenRGBEffectPage::EFCT = EFCT;

    ui->SpeedFrame->hide();
    ui->UserColorFrame->hide();
    ui->Slider2Frame->hide();

    ui->StopButton->setDisabled(true);

    /*---------------------------------*\
    | Fill in top description and name  |
    \*---------------------------------*/
    ui->EffectName->setText(QString().fromStdString(EFCT->EffectDetails.EffectName));
    ui->EffectDesciption->setText(QString().fromStdString(EFCT->EffectDetails.EffectDescription));

    /*-----------------------------------------------*\
    | If maximum speed is greater than Minimum speed  |
    | Set start and stop of speed slider              |
    \*-----------------------------------------------*/
    if (EFCT->EffectDetails.MinSpeed < EFCT->EffectDetails.MaxSpeed)
    {
        ui->SpeedSlider->setMaximum(EFCT->EffectDetails.MaxSpeed);
        ui->SpeedSlider->setMinimum(EFCT->EffectDetails.MinSpeed);
        ui->SpeedFrame->show();
        OpenRGBEffectPage::HasSpeed = true;
    }

    if (EFCT->EffectDetails.MinSlider2Val < EFCT->EffectDetails.MaxSlider2Val)
    {
        ui->Slider2->setMaximum(EFCT->EffectDetails.MaxSlider2Val);
        ui->Slider2->setMinimum(EFCT->EffectDetails.MinSlider2Val);
        ui->Slider2Label->setText(QString().fromStdString(EFCT->EffectDetails.Slider2Name));
        ui->Slider2Frame->show();
    }


    EFCT->DefineExtraOptions(ui->ExtraOptions);
    if (EFCT->EffectDetails.HasCustomWidgets)
    {
        ui->It_Goes_On_The_Bottom->changeSize(0,0,QSizePolicy::Fixed); // Gone
        resize(this->minimumSize());
    }


    if (EFCT->EffectDetails.UserColors > 0)
    {
        ui->UserColorNum->setMaxCount(EFCT->EffectDetails.UserColors);
        for (int UserColorIndex = 0; UserColorIndex < EFCT->EffectDetails.UserColors; UserColorIndex++)
        {
            RGBColor UserColor = ToRGBColor(255,255,255);
            UserColors.push_back(UserColor);
            ui->UserColorNum->addItem("Color " + QString().number(UserColorIndex + 1));
        }
        ui->ColorPreview->setStyleSheet("background: rgb("+ QString().number(255) + "," + QString().number(255) + "," + QString().number(255) + ")");
        ui->UserColorFrame->setHidden(false);

        EFCT->SetUserColors(UserColors);
    }

    OpenRGBEffectPage::StartupSettings();

}

OpenRGBEffectPage::~OpenRGBEffectPage()
{
    delete ui;
}


/*--------*\
| Slots    |
\*--------*/
void OpenRGBEffectPage::on_StartButton_clicked()
{
    OpenRGBEffectTab::SetEffectActive(EFCT);
    EFCT->EffectState(true);

    ui->StartButton->setDisabled(true);
    ui->StopButton->setDisabled(false);
    ui->RunningStatus->setText("Running");
}

void OpenRGBEffectPage::on_StopButton_clicked()
{
    OpenRGBEffectTab::SetEffectUnActive(EFCT);
    EFCT->EffectState(false);

    ui->StartButton->setDisabled(false);
    ui->StopButton->setDisabled(true);
    ui->RunningStatus->setText("Stopped");
}

void OpenRGBEffectPage::on_SpeedSlider_valueChanged(int value)
{
    OpenRGBEffectPage::EFCT->SetSpeed(value);
}

void OpenRGBEffectPage::on_Slider2_valueChanged(int value)
{
    EFCT->Slider2Changed(value);
}

void OpenRGBEffectPage::on_UserColorNum_currentIndexChanged(int NewIndex)
{
    CurrentColor = NewIndex;
    ui->ColorPreview->setStyleSheet("background: rgb("+
                                    QString().number(RGBGetRValue(UserColors[NewIndex])) + "," +
                                    QString().number(RGBGetGValue(UserColors[NewIndex])) + "," +
                                    QString().number(RGBGetBValue(UserColors[NewIndex])) + ")");
}

void OpenRGBEffectPage::on_ColorPickerButton_clicked()
{
    QDialog* GUColor = new QDialog();

    if (ORGBPlugin::DarkTheme)
    {
        QPalette pal;
        pal.setColor(QPalette::WindowText, Qt::white);
        GUColor->setPalette(pal);
        QFile darkTheme(":/windows_dark.qss");
        darkTheme.open(QFile::ReadOnly);
        GUColor->setStyleSheet(darkTheme.readAll());
        darkTheme.close();
    }

    GUColor->setMinimumSize(300,320);

    GUColor->setModal(true);

    QVBoxLayout* MainLayout = new QVBoxLayout(GUColor);

    ColorWheel* CWheel = new ColorWheel(GUColor);
    MainLayout->addWidget(CWheel);

    /*--------------*\
    | Create buttons |
    \*--------------*/
    QHBoxLayout* HoriButtonLayout = new QHBoxLayout();

    QPushButton* AcceptButton = new QPushButton();
    AcceptButton->setText("Set Color");
    GUColor->connect(AcceptButton,SIGNAL(clicked()),GUColor,SLOT(accept()));
    HoriButtonLayout->addWidget(AcceptButton);

    QPushButton* DenyButton = new QPushButton();
    DenyButton->setText("Cancel");
    GUColor->connect(DenyButton,SIGNAL(clicked()),GUColor,SLOT(reject()));
    HoriButtonLayout->addWidget(DenyButton);

    MainLayout->addLayout(HoriButtonLayout);

    bool ReturnState = GUColor->exec();

    if (ReturnState)
    {

        QColor UserColor = CWheel->color();
        int Red   = UserColor.red();
        int Green = UserColor.green();
        int Blue  = UserColor.blue();

        UserColors[CurrentColor] = ToRGBColor(Red,Green,Blue);
        ui->ColorPreview->setStyleSheet("background: rgb("+ QString().number(Red) + "," + QString().number(Green) + "," + QString().number(Blue) + ")");

        EFCT->SetUserColors(OpenRGBEffectPage::UserColors);

        delete GUColor;
    }
    else{}
}

void OpenRGBEffectPage::on_AutoStart_clicked()
{
    if (ui->AutoStart->isChecked())
    {
        AutoStart = true;
    }
    else
    {
        AutoStart = false;
    }
}


/*---------*\
| Settings  |
\*---------*/
void OpenRGBEffectPage::StartupSettings()
{
    json UserSettings = OpenRGBEffectTab::LoadPrevSetting();

    if(UserSettings.contains("Effects"))
    {
        if (UserSettings["Effects"][EFCT->EffectDetails.EffectIndex].contains("EffectName"))
        {
            // Sliders
            int SpeedMax = EFCT->EffectDetails.MaxSpeed;
            int SpeedMin = EFCT->EffectDetails.MinSpeed;
            if (SpeedMin <= UserSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["Speed"] <= SpeedMax)
            {
                ui->SpeedSlider->setValue(UserSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["Speed"]);
            }

            int Slider2Max = EFCT->EffectDetails.MaxSlider2Val;
            int Slider2Min = EFCT->EffectDetails.MinSlider2Val;
            if ( Slider2Min <= UserSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["Slider2Val"] <= Slider2Max)
            {
                ui->Slider2->setValue(UserSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["Slider2Val"]);
            }

            if(UserSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["AutoStart"])
            {
                ui->StartButton->click();
                ui->AutoStart->click();
            }

            // User colors
            std::vector<RGBColor> NewUserColors;
            for (int UserColorIndex = 0; UserColorIndex < EFCT->EffectDetails.UserColors; UserColorIndex++)
            {
                NewUserColors.push_back(UserSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["UserColors"][UserColorIndex]);
            }
            if (EFCT->EffectDetails.UserColors > 0)
            {
                EFCT->SetUserColors(NewUserColors);
                UserColors = NewUserColors;

                /*----------------*\
                | Set color frame  |
                \*----------------*/
                int R = RGBGetRValue(UserColors[CurrentColor]);
                int G = RGBGetGValue(UserColors[CurrentColor]);
                int B = RGBGetBValue(UserColors[CurrentColor]);
                ui->ColorPreview->setStyleSheet("background: rgb("+ QString().number(R) + "," + QString().number(G) + "," + QString().number(B) + ")");
            }

            if (EFCT->EffectDetails.HasCustomSettings)
            {
                EFCT->LoadCustomSettings(UserSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["CustomSettings"]);
            }
        }
    }
}

void OpenRGBEffectPage::on_SaveSettings_clicked()
{
    json PrevSettings = OpenRGBEffectTab::LoadPrevSetting();

    PrevSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectName"] = EFCT->EffectDetails.EffectName;

    std::vector<RGBColor> UserColors = EFCT->GetUserColors();
    for (int UserColorIndex = 0; UserColorIndex < EFCT->EffectDetails.UserColors; UserColorIndex++)
    {
        PrevSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["UserColors"][UserColorIndex] = UserColors[UserColorIndex];
    }

    std::vector<OwnedControllerAndZones> ToPass = OpenRGBEffectTab::GetToPass(EFCT->EffectDetails.EffectIndex);

    PrevSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["Controllers"] = {};

    int AddedDevices = 0;
    for (int ControllerID = 0; ControllerID < (int)ToPass.size(); ControllerID++)
    {
        if (ToPass[ControllerID].OwnedZones.size() > 0)
        {
            PrevSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["Controllers"][AddedDevices]["ControllerName"]    = ToPass[ControllerID].Controller->name;
            PrevSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["Controllers"][AddedDevices]["ControllerDescription"] = ToPass[ControllerID].Controller->description;
            PrevSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["Controllers"][AddedDevices]["ControllerLocation"]    = ToPass[ControllerID].Controller->location;
            PrevSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["Controllers"][AddedDevices]["ControllerSerial"]      = ToPass[ControllerID].Controller->serial;
            PrevSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["Controllers"][AddedDevices]["ControllerLEDCount"]    = ToPass[ControllerID].Controller->colors.size();

            for(int ZoneID = 0; ZoneID < (int)ToPass[ControllerID].OwnedZones.size(); ZoneID++)
            {
                PrevSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["Controllers"][AddedDevices]["SelectedZones"][ZoneID] = ToPass[ControllerID].OwnedZones[ZoneID];
            }
            AddedDevices += 1;
        }
    }

    PrevSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["Speed"] = EFCT->GetSpeed();
    PrevSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["Slider2Val"] = EFCT->GetSlider2Val();
    PrevSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["AutoStart"] = AutoStart;

    if (EFCT->EffectDetails.HasCustomSettings)
    {
        PrevSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["CustomSettings"] =
        EFCT->SaveCustomSettings
        (
            PrevSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["CustomSettings"]
        );
    }

    std::ofstream EffectFile((ORGBPlugin::RMPointer->GetConfigurationDirectory() + "/plugins/EffectSettings.json"), std::ios::out | std::ios::binary);
    if(EffectFile)
    {
        try{ EffectFile << PrevSettings.dump(4); }
        catch(std::exception e){}
        EffectFile.close();
    }
}
