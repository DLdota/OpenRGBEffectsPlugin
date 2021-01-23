#include "OpenRGBEffectPage.h"

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
    }

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
    QColorDialog* GUColor = new QColorDialog();

    GUColor->setPalette(this->palette());
    GUColor->setModal(true);

    QColor UserColor = GUColor->getColor();
    int Red = UserColor.red();
    int Green = UserColor.green();
    int Blue = UserColor.blue();

    unsigned int rgb = (  Red<<16 )|( Green<<8 )|( Blue );

    UserColors[CurrentColor] = rgb;
    ui->ColorPreview->setStyleSheet("background: rgb("+ QString().number(Red) + "," + QString().number(Green) + "," + QString().number(Blue) + ")");

    EFCT->SetUserColors(OpenRGBEffectPage::UserColors);

}
