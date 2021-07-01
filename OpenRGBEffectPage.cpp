#include "OpenRGBEffectPage.h"
#include "OpenRGBEffectSettings.h"
#include "OpenRGBEffectsPlugin.h"
#include "EffectManager.h"
#include "ColorUtils.h"
#include "LivePreviewController.h"
#include <QDialog>
#include <QFile>
#include <iomanip>
#include <sstream>
#include <QInputDialog>
#include <QTextEdit>
#include <QClipboard>

OpenRGBEffectPage::OpenRGBEffectPage(QWidget *parent, RGBEffect* effect):
    QWidget(parent),
    ui(new Ui::OpenRGBEffectPage),
    effect(effect),
    speeds({1,2,3,4,5,6,7,8,10,15,20,25,30,40,50,60})
{
    ui->setupUi(this);

    /*-----------------------------------------------*\
    | Extra options and custom widgets                |
    \*-----------------------------------------------*/
    effect->DefineExtraOptions(ui->ExtraOptions);

    InitUi();

    connect(effect, &RGBEffect::TimeMeasured, [=](float time, int duration){
        int hour = duration / 3600;
        int minute = (duration % 3600) / 60;
        int second = duration % 60;

        std::stringstream stream;
        stream << std::fixed << std::setprecision(1) << time << "ms \n"
                  << (hour   < 10 ? "0" : "") << hour    << ":"
                  << (minute < 10 ? "0" : "") << minute  << ":"
                  << (second < 10 ? "0" : "") << second;

        ui->time_measure->setText(QString::fromStdString(stream.str()));
    });
}

OpenRGBEffectPage::~OpenRGBEffectPage()
{
    delete ui;
}

void OpenRGBEffectPage::InitUi()
{    
    colors_layout = new QHBoxLayout();
    ui->Colors->setLayout(colors_layout);

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
    ui->OnlyFirst->setCheckState(effect->IsOnlyFirstColorEnabled()? Qt::Checked : Qt::Unchecked);
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


    if (effect->EffectDetails.UserColors > 0)
    {
        QLayoutItem *child;

        while ((child = colors_layout->takeAt(0)) != 0) {
            delete child->widget();
        }

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
            color_picker->SetRGBColor(colors[i]);

            ColorPickers.push_back(color_picker);

            connect(color_picker, &ColorPicker::ColorSelected, [=](QColor color){
                UserColors[i] = ColorUtils::fromQColor(color);
                effect->SetUserColors(UserColors);
            });

            colors_layout->addWidget(color_picker);
        }

        effect->SetUserColors(UserColors);
    }

    effect->SetRandomColorsEnabled(effect->IsRandomColorsEnabled());

    ui->FPS_slider->setMinimum(0);
    ui->FPS_slider->setMaximum(speeds.size() - 1);

    std::vector<unsigned int>::iterator it = std::find(speeds.begin(), speeds.end(), effect->GetFPS());

    if( it != speeds.end() )
    {
        int index = std::distance(speeds.begin(), it);
        ui->FPS_slider->setValue(index);
    }

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

void OpenRGBEffectPage::OpenPreview()
{
    preview_dialog = new QDialog(this);
    preview_dialog->setAttribute(Qt::WA_DeleteOnClose);
    preview_dialog->setModal(false);
    preview_dialog->setWindowTitle(QString::fromStdString(effect->EffectDetails.EffectName + " preview"));

    if (OpenRGBEffectsPlugin::DarkTheme)
    {
        QPalette pal;
        pal.setColor(QPalette::WindowText, Qt::white);
        preview_dialog->setPalette(pal);
        QFile dark_theme(":/windows_dark.qss");
        dark_theme.open(QFile::ReadOnly);
        preview_dialog->setStyleSheet(dark_theme.readAll());
        dark_theme.close();
    }

    preview_dialog->setMinimumSize(64,64);

    QVBoxLayout* dialog_layout = new QVBoxLayout(preview_dialog);

    LivePreviewController* preview = new LivePreviewController(preview_dialog);
    preview_zone = new ControllerZone(preview, 0, false);
    EffectManager::Get()->AddPreview(effect, preview_zone);

    dialog_layout->addWidget(preview);

    preview_dialog->show();

    ui->PreviewButton->setDisabled(true);

    connect(preview, &LivePreviewController::ReversedChanged, [=](bool state){
        preview_zone->reverse = state;
    });

    connect(preview_dialog, &QDialog::finished, [=](){
        ui->PreviewButton->setDisabled(false);
        EffectManager::Get()->RemovePreview(effect);
    });
}

void OpenRGBEffectPage::on_StartButton_clicked()
{
    StartEffect();
}

void OpenRGBEffectPage::on_StopButton_clicked()
{
    StopEffect();
}

void OpenRGBEffectPage::on_PreviewButton_clicked()
{
    OpenPreview();
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

void OpenRGBEffectPage::on_save_pattern_clicked()
{
    QString filename = QInputDialog::getText(
                nullptr, "Save pattern to file...", "Choose a filename",
                QLineEdit::Normal, QString("my-pattern")).trimmed();

    json effect_settings = ToJson();

    OpenRGBEffectSettings::SaveEffectPattern(effect_settings, effect->EffectDetails.EffectClassName, filename.toStdString());
}

void OpenRGBEffectPage::on_load_pattern_clicked()
{
    std::vector<std::string> filenames = OpenRGBEffectSettings::ListPattern(effect->EffectDetails.EffectClassName);

    QStringList file_list;

    for(std::string filename : filenames)
    {
        file_list << QString::fromUtf8(filename.c_str());
    }

    if(file_list.empty())
    {
        return;
    }

    QInputDialog *inp = new QInputDialog(this);

    inp->setOptions(QInputDialog::UseListViewForComboBoxItems);
    inp->setComboBoxItems(file_list);
    inp->setWindowTitle("Load pattern from file...");
    inp->setLabelText("Choose a pattern file from this list:");

    if(!inp->exec()){
        return;
    }

    QString filename = inp->textValue();

    json effect_settings = OpenRGBEffectSettings::LoadPattern(effect->EffectDetails.EffectClassName, filename.toStdString());

    try {
        ApplyJson(effect_settings);
    }  catch (const std::exception & e) {
        printf("Cannot apply effect settings, reason: %s\n", e.what());
    }
}

json OpenRGBEffectPage::ToJson()
{
    json effect_settings;

    effect_settings["EffectClassName"] = effect->EffectDetails.EffectClassName;
    effect_settings["FPS"] = effect->GetFPS();
    effect_settings["Speed"] = effect->GetSpeed();
    effect_settings["Slider2Val"] = effect->GetSlider2Val();
    effect_settings["RandomColors"] = effect->IsRandomColorsEnabled();
    effect_settings["AllowOnlyFirst"] = effect->IsOnlyFirstColorEnabled();

    std::vector<RGBColor> colors = effect->GetUserColors();

    for (unsigned int c = 0; c < colors.size(); c++)
    {
        effect_settings["UserColors"][c] = colors[c];
    }

    if (effect->EffectDetails.HasCustomSettings)
    {
        json j;
        effect_settings["CustomSettings"] = effect->SaveCustomSettings(j);
    }

    return effect_settings;
}

void OpenRGBEffectPage::ApplyJson(json effect_settings)
{
    // Update effect values
    std::vector<RGBColor> colors;

    for(unsigned int color : effect_settings["UserColors"])
    {
        colors.push_back(color);
    }

    effect->SetFPS(effect_settings["FPS"]);
    effect->SetUserColors(colors);
    effect->SetSpeed(effect_settings["Speed"]);
    effect->SetSlider2Val(effect_settings["Slider2Val"]);
    effect->SetRandomColorsEnabled(effect_settings["RandomColors"]);
    effect->SetOnlyFirstColorEnabled(effect_settings["AllowOnlyFirst"]);

    if(effect_settings.contains("CustomSettings"))
    {
        effect->LoadCustomSettings(effect_settings["CustomSettings"]);
    }

    // Update UI components
    ui->RandomCheckbox->setCheckState(effect->IsRandomColorsEnabled()? Qt::Checked : Qt::Unchecked);
    ui->OnlyFirst->setCheckState(effect->IsOnlyFirstColorEnabled()? Qt::Checked : Qt::Unchecked);
    ui->SpeedSlider->setValue(effect->GetSpeed());
    ui->Slider2->setValue(effect->GetSlider2Val());

    for(int i = 0; i < colors_layout->count(); i++)
    {
        ColorPicker* picker = dynamic_cast<ColorPicker*>(colors_layout->itemAt(i)->widget());
        picker->SetRGBColor(colors[i]);
    }
}

void OpenRGBEffectPage::on_edit_pattern_clicked()
{
    QDialog* dialog = new QDialog();

    if (OpenRGBEffectsPlugin::DarkTheme)
    {
        QPalette pal;
        pal.setColor(QPalette::WindowText, Qt::white);
        dialog->setPalette(pal);
        QFile dark_theme(":/windows_dark.qss");
        dark_theme.open(QFile::ReadOnly);
        dialog->setStyleSheet(dark_theme.readAll());
        dark_theme.close();
    }

    dialog->setMinimumSize(300,320);
    dialog->setModal(true);
    dialog->setWindowTitle("Edit or share your settings.");

    QVBoxLayout* dialog_layout = new QVBoxLayout(dialog);

    QTextEdit* text_edit = new QTextEdit(dialog);

    text_edit->setText(QString::fromStdString(ToJson().dump(4)));

    dialog_layout->addWidget(text_edit);

    QHBoxLayout* buttons_layout = new QHBoxLayout();

    QPushButton* cancel_button = new QPushButton();
    cancel_button->setText("Cancel");
    dialog->connect(cancel_button,SIGNAL(clicked()),dialog,SLOT(reject()));
    buttons_layout->addWidget(cancel_button);

    QPushButton* copy_button = new QPushButton();
    copy_button->setText("Copy to clipboard");

    dialog->connect(copy_button, &QPushButton::clicked, [=](){
        std::string text =
                "**" + effect->EffectDetails.EffectClassName + "**" + "\n" +
                "```" +
                text_edit->toPlainText().toStdString() +
                "```" ;

        QClipboard *clipboard = QGuiApplication::clipboard();
        clipboard->setText(QString::fromStdString(text));
    });

    buttons_layout->addWidget(copy_button);

    QPushButton* accept_button = new QPushButton();
    accept_button->setText("Apply");
    dialog->connect(accept_button,SIGNAL(clicked()),dialog,SLOT(accept()));
    buttons_layout->addWidget(accept_button);

    dialog_layout->addLayout(buttons_layout);

    if (dialog->exec())
    {
        QString text = text_edit->toPlainText();

        if (!text.isEmpty())
        {
            try {
                ApplyJson(json::parse(text.toStdString()));
            } catch (const std::exception & e) {
                printf("Cannot apply effect settings, reason: %s\n", e.what());
            }
        }
    }
}
