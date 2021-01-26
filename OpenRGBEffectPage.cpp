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

    EFCT->DefineExtraOptions(ui->OptionFrame);

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

    ui->StartButton->setDisabled(true);
    ui->StopButton->setDisabled(false);
    ui->RunningStatus->setText("Running");
}

void OpenRGBEffectPage::on_StopButton_clicked()
{
    OpenRGBEffectTab::SetEffectUnActive(EFCT);

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
    int Red   = (UserColors[NewIndex] & 0x00ff0000) >> 16;
    int Green = (UserColors[NewIndex] & 0x0000ff00) >> 8;
    int Blue  = (UserColors[NewIndex] & 0x000000ff);

    ui->ColorPreview->setStyleSheet("background: rgb("+ QString().number(Red) + "," + QString().number(Green) + "," + QString().number(Blue) + ")");
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
        int Red = UserColor.red();
        int Green = UserColor.green();
        int Blue = UserColor.blue();

        unsigned int rgb = (  Red<<16 )|( Green<<8 )|( Blue );

        UserColors[CurrentColor] = rgb;
        ui->ColorPreview->setStyleSheet("background: rgb("+ QString().number(Red) + "," + QString().number(Green) + "," + QString().number(Blue) + ")");

        EFCT->SetUserColors(OpenRGBEffectPage::UserColors);

        delete GUColor;
    }
    else{}
}


/*---------*\
| Settings  |
\*---------*/
void OpenRGBEffectPage::StartupSettings()
{
    json UserSettings = LoadPrevSetting();

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
                int R = (UserColors[CurrentColor] & 0x00ff0000) >> 16;
                int G = (UserColors[CurrentColor] & 0x0000ff00) >> 8;
                int B = (UserColors[CurrentColor] & 0x000000ff);
                ui->ColorPreview->setStyleSheet("background: rgb("+ QString().number(R) + "," + QString().number(G) + "," + QString().number(B) + ")");
            }

        }
    }
}

json OpenRGBEffectPage::LoadPrevSetting()
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

void OpenRGBEffectPage::on_SaveSettings_clicked()
{
    json PrevSettings = LoadPrevSetting();

    PrevSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectName"] = EFCT->EffectDetails.EffectName;

    std::vector<RGBColor> UserColors = EFCT->GetUserColors();
    for (int UserColorIndex = 0; UserColorIndex < EFCT->EffectDetails.UserColors; UserColorIndex++)
    {
        PrevSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["UserColors"][UserColorIndex] = UserColors[UserColorIndex];
    }

    PrevSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["Speed"] = EFCT->GetSpeed();
    PrevSettings["Effects"][EFCT->EffectDetails.EffectIndex]["EffectSettings"]["Slider2Val"] = EFCT->GetSlider2Val();

    std::ofstream EffectFile((ORGBPlugin::RMPointer->GetConfigurationDirectory() + "/plugins/EffectSettings.json"), std::ios::out | std::ios::binary);
    if(EffectFile)
    {
        try
        {
            EffectFile << PrevSettings.dump(4);
        }
        catch(std::exception e)
        {

        }

        EffectFile.close();
    }
}
