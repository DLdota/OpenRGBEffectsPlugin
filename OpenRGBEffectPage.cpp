#include "ORGBEffectPlugin.h"
#include "OpenRGBEffectPage.h"
#include "EffectManager.h"
#include "ColorPicker.h"
#include "ColorWheel.h"

/*-----------------------*\
| Constructor/Destructor  |
\*-----------------------*/
OpenRGBEffectPage::OpenRGBEffectPage(QWidget *parent, RGBEffect* Effect): QWidget(parent), ui(new Ui::OpenRGBEffectPage), EFCT(Effect)
{
    ui->setupUi(this);

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
        HasSpeed = true;
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


    ui->UserColorFrame->setVisible(EFCT->EffectDetails.UserColors > 0);
    ui->OnlyFirst->setVisible(EFCT->EffectDetails.AllowOnlyFirst);

    ui->Colors->setLayout(new QHBoxLayout);

    if (EFCT->EffectDetails.UserColors > 0)
    {
        ui->OnlyFirst->setVisible(EFCT->EffectDetails.AllowOnlyFirst);

        for (int UserColorIndex = 0; UserColorIndex < EFCT->EffectDetails.UserColors; UserColorIndex++)
        {
            RGBColor UserColor = ToRGBColor(255,255,255);
            UserColors.push_back(UserColor);

            ColorPicker* color_picker = new ColorPicker();
            ColorPickers.push_back(color_picker);

            connect(color_picker, &ColorPicker::ColorSelected, [=](QColor color){
               int Red   = color.red();
               int Green = color.green();
               int Blue  = color.blue();
               UserColors[UserColorIndex] = ToRGBColor(Red,Green,Blue);
               EFCT->SetUserColors(UserColors);
            });

            ui->Colors->layout()->addWidget(color_picker);
        }

        EFCT->SetUserColors(UserColors);
    }

    StartupSettings();

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
    EffectManager::Get()->SetEffectActive(EFCT);
    EFCT->EffectState(true);

    ui->StartButton->setDisabled(true);
    ui->StopButton->setDisabled(false);
    ui->RunningStatus->setText("Running");
}

void OpenRGBEffectPage::on_StopButton_clicked()
{
    EffectManager::Get()->SetEffectUnActive(EFCT);
    EFCT->EffectState(false);

    ui->StartButton->setDisabled(false);
    ui->StopButton->setDisabled(true);
    ui->RunningStatus->setText("Stopped");
}

void OpenRGBEffectPage::on_SpeedSlider_valueChanged(int value)
{
    EFCT->SetSpeed(value);
}

void OpenRGBEffectPage::on_Slider2_valueChanged(int value)
{
    EFCT->Slider2Changed(value);
}

void OpenRGBEffectPage::on_AutoStart_clicked()
{
    AutoStart = ui->AutoStart->isChecked();
}

void OpenRGBEffectPage::on_RandomCheckbox_clicked()
{
    EFCT->ToggleRandomColors(ui->RandomCheckbox->isChecked());
}

void OpenRGBEffectPage::on_OnlyFirst_clicked()
{
    EFCT->OnlyFirstChange(ui->OnlyFirst->isChecked());
}

/*---------*\
| Settings  |
\*---------*/
void OpenRGBEffectPage::StartupSettings()
{
    json UserSettings = OpenRGBEffectSettings::LoadUserSettings();

    if(UserSettings.contains("Effects"))
    {
        if (UserSettings["Effects"][EFCT->EffectDetails.EffectIndex].contains("EffectName"))
        {
            // Sliders
            int SpeedMax = EFCT->EffectDetails.MaxSpeed;
            int SpeedMin = EFCT->EffectDetails.MinSpeed;
            if (UserSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"].contains("Speed"))
            {
                int Speed = UserSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["Speed"];
                if (SpeedMin <= Speed && Speed <= SpeedMax)
                {
                    ui->SpeedSlider->setValue(UserSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["Speed"]);
                }
            }

            int Slider2Max = EFCT->EffectDetails.MaxSlider2Val;
            int Slider2Min = EFCT->EffectDetails.MinSlider2Val;
            if (UserSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"].contains("Slider2Val"))
            {
                int Slider2Val = UserSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["Slider2Val"];
                if ( Slider2Min <= Slider2Val && Slider2Val <= Slider2Max)
                {
                    ui->Slider2->setValue(UserSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["Slider2Val"]);
                }
            }

            // User colors
            if (UserSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"].contains("UserColors"))
            {
                std::vector<RGBColor> NewUserColors;

                for (int UserColorIndex = 0; UserColorIndex < EFCT->EffectDetails.UserColors; UserColorIndex++)
                {
                    NewUserColors.push_back(UserSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["UserColors"][UserColorIndex]);
                }

                if (EFCT->EffectDetails.UserColors > 0)
                {
                    EFCT->SetUserColors(NewUserColors);
                    UserColors = NewUserColors;

                    for(unsigned int i = 0; i < ColorPickers.size(); i++)
                    {
                        QColor color(RGBGetRValue(UserColors[i]), RGBGetGValue(UserColors[i]), RGBGetBValue(UserColors[i]));
                        ColorPickers[i]->SetColor(color);
                    }
                }
            }

            if (UserSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"].contains("RandomColors"))
            {
                bool RandomColors = UserSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["RandomColors"];

                if (RandomColors)
                {
                    ui->RandomCheckbox->click();
                }
            }

            if(EFCT->EffectDetails.HasCustomSettings &&
                    UserSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"].contains("CustomSettings"))
            {
                EFCT->LoadCustomSettings(UserSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["CustomSettings"]);
            }

            if (UserSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"].contains("AutoStart") &&
                    UserSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["AutoStart"])
            {
                ui->AutoStart->click();
                ui->StartButton->click();
            }

        }
    }
}

void OpenRGBEffectPage::on_SaveSettings_clicked()
{
    json PrevSettings = OpenRGBEffectSettings::LoadUserSettings();

    PrevSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectName"] = EFCT->EffectDetails.EffectName;

    std::vector<RGBColor> UserColors = EFCT->GetUserColors();

    for (int UserColorIndex = 0; UserColorIndex < EFCT->EffectDetails.UserColors; UserColorIndex++)
    {
        PrevSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["UserColors"][UserColorIndex] = UserColors[UserColorIndex];
    }

    std::vector<OwnedControllerAndZones> ToPass = EffectManager::Get()->GetToPass(EFCT->EffectDetails.EffectIndex);

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
    PrevSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["RandomColors"] = ui->RandomCheckbox->isChecked();

    if (EFCT->EffectDetails.HasCustomSettings)
    {
        PrevSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["CustomSettings"] =
                EFCT->SaveCustomSettings
                (
                    PrevSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["CustomSettings"]
                );
    }

    OpenRGBEffectSettings::SaveUserSettings(PrevSettings);
}
