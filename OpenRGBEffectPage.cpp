#include "OpenRGBEffectPage.h"
#include "OpenRGBEffectSettings.h"
#include "EffectManager.h"

OpenRGBEffectPage::OpenRGBEffectPage(QWidget *parent, RGBEffect* effect):
    QWidget(parent),
    ui(new Ui::OpenRGBEffectPage),
    speeds({1,2,3,4,5,6,7,8,10,15,20,25,30,40,50,60})
{
    this->effect = effect;

    ui->setupUi(this);

    InitUi();
}

OpenRGBEffectPage::~OpenRGBEffectPage()
{
    delete ui;
}

void OpenRGBEffectPage::InitUi()
{
    ui->SpeedFrame->hide();
    ui->UserColorFrame->hide();
    ui->Slider2Frame->hide();

    ui->StopButton->setDisabled(true);

    /*---------------------------------*\
    | Fill in top description and name  |
    \*---------------------------------*/
    ui->EffectName->setText(QString().fromStdString(effect->EffectDetails.EffectName));
    ui->EffectDesciption->setText(QString().fromStdString(effect->EffectDetails.EffectDescription));

    ui->AutoStart->setCheckState(effect->IsAutoStart()? Qt::Checked : Qt::Unchecked);
    ui->RandomCheckbox->setCheckState(effect->IsRandomColorsEnabled()? Qt::Checked : Qt::Unchecked);
    ui->SpeedSlider->setValue(effect->GetSpeed());
    ui->Slider2->setValue(effect->GetSlider2Val());

    /*-----------------------------------------------*\
    | Speed slider + extra slider                     |
    \*-----------------------------------------------*/
    if (effect->EffectDetails.MinSpeed < effect->EffectDetails.MaxSpeed)
    {
        ui->SpeedSlider->setMaximum(effect->EffectDetails.MaxSpeed);
        ui->SpeedSlider->setMinimum(effect->EffectDetails.MinSpeed);
        ui->SpeedFrame->show();
    }

    if (effect->EffectDetails.MinSlider2Val < effect->EffectDetails.MaxSlider2Val)
    {
        ui->Slider2->setMaximum(effect->EffectDetails.MaxSlider2Val);
        ui->Slider2->setMinimum(effect->EffectDetails.MinSlider2Val);
        ui->Slider2Label->setText(QString().fromStdString(effect->EffectDetails.Slider2Name));
        ui->Slider2Frame->show();
    }

    if (effect->EffectDetails.HasCustomWidgets)
    {
        ui->It_Goes_On_The_Bottom->changeSize(0,0,QSizePolicy::Fixed); // Gone
        resize(this->minimumSize());
    }

    /*-----------------------------------------------*\
    | Colors                                          |
    \*-----------------------------------------------*/
    ui->UserColorFrame->setVisible(effect->EffectDetails.UserColors > 0);
    ui->OnlyFirst->setVisible(effect->EffectDetails.AllowOnlyFirst);

    ui->Colors->setLayout(new QHBoxLayout);

    if (effect->EffectDetails.UserColors > 0)
    {
        std::vector<RGBColor> colors = effect->GetUserColors();

        if(colors.size() != effect->EffectDetails.UserColors)
        {
            colors.resize(effect->EffectDetails.UserColors);
        }

        for (unsigned int i = 0; i < colors.size(); i++)
        {
            RGBColor UserColor = colors[i];
            UserColors.push_back(UserColor);            

            ColorPicker* color_picker = new ColorPicker();
            color_picker->SetColor(QColor(RGBGetRValue(colors[i]), RGBGetGValue(colors[i]), RGBGetBValue(colors[i])));

            ColorPickers.push_back(color_picker);

            connect(color_picker, &ColorPicker::ColorSelected, [=](QColor color){
               int Red   = color.red();
               int Green = color.green();
               int Blue  = color.blue();
               UserColors[i] = ToRGBColor(Red,Green,Blue);
               effect->SetUserColors(UserColors);
            });

            ui->Colors->layout()->addWidget(color_picker);
        }

        effect->SetUserColors(UserColors);
    }



    ui->FPS_slider->setMinimum(0);
    ui->FPS_slider->setMaximum(speeds.size() - 1);

    std::vector<unsigned int>::iterator it = std::find(speeds.begin(), speeds.end(), effect->GetFPS());

    if( it != speeds.end() )
    {
        int index = std::distance(speeds.begin(), it);
        ui->FPS_slider->setValue(index);
    }

    /*-----------------------------------------------*\
    | Extra options and custom widgets                |
    \*-----------------------------------------------*/
    effect->DefineExtraOptions(ui->ExtraOptions);

    if(effect->IsAutoStart())
    {
        StartEffect();
    }
}

RGBEffect* OpenRGBEffectPage::GetEffect()
{
    return effect;
}

void OpenRGBEffectPage::StartEffect()
{
    EffectManager::Get()->SetEffectActive(effect);

    ui->StartButton->setDisabled(true);
    ui->StopButton->setDisabled(false);
    ui->RunningStatus->setText("Running");

    emit EffectState(true);
}

void OpenRGBEffectPage::StopEffect()
{
    EffectManager::Get()->SetEffectUnActive(effect);

    ui->StartButton->setDisabled(false);
    ui->StopButton->setDisabled(true);
    ui->RunningStatus->setText("Stopped");

    emit EffectState(false);
}

void OpenRGBEffectPage::on_StartButton_clicked()
{
    StartEffect();
}

void OpenRGBEffectPage::on_StopButton_clicked()
{
    StopEffect();
}

void OpenRGBEffectPage::on_SpeedSlider_valueChanged(int value)
{
    effect->SetSpeed(value);
}

void OpenRGBEffectPage::on_Slider2_valueChanged(int value)
{
    effect->SetSlider2Val(value);
}

void OpenRGBEffectPage::on_FPS_slider_valueChanged(int value)
{
    int FPS = speeds[value];

    effect->SetFPS(FPS);
    ui->FPS_value->setText(QString::number(FPS));
}

void OpenRGBEffectPage::on_AutoStart_clicked()
{
    AutoStart = ui->AutoStart->isChecked();
    effect->SetAutoStart(AutoStart);
}

void OpenRGBEffectPage::on_RandomCheckbox_clicked()
{
    effect->SetRandomColorsEnabled(ui->RandomCheckbox->isChecked());
}

void OpenRGBEffectPage::on_OnlyFirst_clicked()
{
    effect->SetOnlyFirstColorEnabled(ui->OnlyFirst->isChecked());
}
