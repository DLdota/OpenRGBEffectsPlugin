#include "OpenRGBEffectPage.h"
#include "OpenRGBEffectSettings.h"
#include "OpenRGBPluginsFont.h"
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
#include <QListWidget>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>

OpenRGBEffectPage::OpenRGBEffectPage(QWidget *parent, RGBEffect* effect):
    QWidget(parent),
    ui(new Ui::OpenRGBEffectPage),
    effect(effect)
{
    ui->setupUi(this);

    ui->EffectDesciption->setVisible(false);
    ui->time_measure->setVisible(false);

    ui->preview->setFont(OpenRGBPluginsFont::GetFont());
    ui->toggle_info->setFont(OpenRGBPluginsFont::GetFont());
    ui->preview->setText(OpenRGBPluginsFont::icon(OpenRGBPluginsFont::tv));
    ui->toggle_info->setText(OpenRGBPluginsFont::icon(OpenRGBPluginsFont::chevron_down));

    /*-----------------------------------------------*\
    | Extra options and custom widgets                |
    \*-----------------------------------------------*/
    ui->ExtraOptions->addWidget(effect);
    ui->ExtraOptions->setMargin(0);
    effect->setParent(ui->extra_settings);

    InitUi();

    connect(effect, &RGBEffect::TimeMeasured, [=](float time, int duration){
        int hour = duration / 3600;
        int minute = (duration % 3600) / 60;
        int second = duration % 60;

        std::stringstream stream;
        stream << "Uptime: "
               << (hour   < 10 ? "0" : "") << hour    << ":"
               << (minute < 10 ? "0" : "") << minute  << ":"
               << (second < 10 ? "0" : "") << second  << " - "
               << "Draw time: " << std::fixed << std::setprecision(1) << time << "ms \n";

        ui->time_measure->setText(QString::fromStdString(stream.str()));
    });

    QMenu* main_menu = new QMenu(this);
    ui->main_menu->setMenu(main_menu);

    QAction* save_pattern = new QAction("Save pattern", this);
    connect(save_pattern, &QAction::triggered, this, &OpenRGBEffectPage::SavePatternAction);
    main_menu->addAction(save_pattern);

    QAction* load_pattern = new QAction("Load pattern", this);
    connect(load_pattern, &QAction::triggered, this, &OpenRGBEffectPage::LoadPatternAction);
    main_menu->addAction(load_pattern);

    QAction* edit_pattern = new QAction("Edit pattern", this);
    connect(edit_pattern, &QAction::triggered, this, &OpenRGBEffectPage::EditPatternAction);
    main_menu->addAction(edit_pattern);


    QAction* open_pattern_folder = new QAction("Open patterns folder", this);
    connect(open_pattern_folder, &QAction::triggered, this, &OpenRGBEffectPage::OpenPatternsFolder);
    main_menu->addAction(open_pattern_folder);
}

OpenRGBEffectPage::~OpenRGBEffectPage()
{
    delete ui;
}

void OpenRGBEffectPage::InitUi()
{    
    colors_layout = new QHBoxLayout();
    ui->Colors->setLayout(colors_layout);

    ui->SpeedLabel->hide();
    ui->SpeedSlider->hide();

    ui->Colors->hide();
    ui->ColorsLabel->hide();
    ui->ColorsSettingsLabel->hide();
    ui->RandomCheckbox->hide();
    ui->OnlyFirst->hide();

    ui->Slider2Label->hide();
    ui->Slider2->hide();

    /*---------------------------------*\
    | Fill in top description and name  |
    \*---------------------------------*/
    ui->EffectName->setText(QString().fromStdString(effect->EffectDetails.EffectName));
    ui->EffectDesciption->setText(QString().fromStdString(effect->EffectDetails.EffectDescription));

    ui->RandomCheckbox->setCheckState(effect->IsRandomColorsEnabled()? Qt::Checked : Qt::Unchecked);
    ui->OnlyFirst->setCheckState(effect->IsOnlyFirstColorEnabled()? Qt::Checked : Qt::Unchecked);

    int speed = effect->GetSpeed();
    int slider2Val = effect->GetSlider2Val();

    /*-----------------------------------------------*\
    | Speed slider + extra slider                     |
    \*-----------------------------------------------*/
    if (effect->EffectDetails.MinSpeed < effect->EffectDetails.MaxSpeed)
    {
        ui->SpeedSlider->setMaximum(effect->EffectDetails.MaxSpeed);
        ui->SpeedSlider->setMinimum(effect->EffectDetails.MinSpeed);
        ui->SpeedLabel->show();
        ui->SpeedSlider->show();
    }

    ui->SpeedSlider->setValue(speed);

    if (effect->EffectDetails.MinSlider2Val < effect->EffectDetails.MaxSlider2Val)
    {
        ui->Slider2->setMaximum(effect->EffectDetails.MaxSlider2Val);
        ui->Slider2->setMinimum(effect->EffectDetails.MinSlider2Val);
        ui->Slider2Label->setText(QString().fromStdString(effect->EffectDetails.Slider2Name));
        ui->Slider2Label->show();
        ui->Slider2->show();
    }

    ui->Slider2->setValue(slider2Val);

    ui->Brightness_slider->setValue(effect->GetBrightness());

    /*-----------------------------------------------*\
    | Colors                                          |
    \*-----------------------------------------------*/
    bool show_colors = effect->EffectDetails.UserColors > 0;
    ui->Colors->setVisible(show_colors);
    ui->ColorsLabel->setVisible(show_colors);

    ui->ColorsSettingsLabel->setVisible(effect->EffectDetails.SupportsRandom || effect->EffectDetails.AllowOnlyFirst);
    ui->RandomCheckbox->setVisible(effect->EffectDetails.SupportsRandom);
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

        colors_layout->addStretch();

        effect->SetUserColors(UserColors);
    }

    ui->Temperature->setValue(effect->GetTemperature());
    ui->Tint->setValue(effect->GetTint());

    effect->SetRandomColorsEnabled(effect->IsRandomColorsEnabled());

    ui->FPS_slider->setValue(effect->GetFPS());

    ui->extra_settings->setVisible(effect->EffectDetails.HasCustomSettings);

    if(!effect->EffectDetails.ExpandCustomSettings)
    {
        ((QVBoxLayout*)layout())->addStretch(100);
    }

    if(effect->IsAutoStart())
    {
        StartEffect();
    }
}

void OpenRGBEffectPage::SetPreviewButtonVisible(bool value)
{
    ui->preview->setVisible(value);
}

void OpenRGBEffectPage::SetFPSSliderVisible(bool value)
{
    ui->FPS_label->setVisible(value);
    ui->FPS_slider->setVisible(value);
}

RGBEffect* OpenRGBEffectPage::GetEffect()
{
    return effect;
}

void OpenRGBEffectPage::StartEffect()
{
    EffectManager::Get()->SetEffectActive(effect);

    emit EffectState(true);
}

void OpenRGBEffectPage::StopEffect()
{
    EffectManager::Get()->SetEffectUnActive(effect);

    emit EffectState(false);
}

void OpenRGBEffectPage::OpenPreview()
{
    preview_dialog = new QDialog(this);

    preview_dialog->setAttribute(Qt::WA_DeleteOnClose);
    preview_dialog->setModal(false);
    preview_dialog->setWindowTitle(QString::fromStdString(effect->EffectDetails.EffectName + " preview"));
    preview_dialog->setWindowFlags(preview_dialog->windowFlags() |
                                   Qt::CustomizeWindowHint       |
                                   Qt::WindowMinimizeButtonHint  |
                                   Qt::WindowMaximizeButtonHint  |
                                   Qt::WindowCloseButtonHint     );

    preview_dialog->setMinimumSize(256,256);

    QVBoxLayout* dialog_layout = new QVBoxLayout(preview_dialog);
    LivePreviewController* preview = new LivePreviewController(preview_dialog);
    preview_zone = new ControllerZone(preview, 0, false, 100, false);
    EffectManager::Get()->AddPreview(effect, preview_zone);

    dialog_layout->addWidget(preview);

    preview_dialog->show();

    ui->preview->setDisabled(true);

    connect(preview, &LivePreviewController::ReversedChanged, [=](bool state){
        preview_zone->reverse = state;
    });

    connect(preview_dialog, &QDialog::finished, [=](){
        ui->preview->setDisabled(false);
        EffectManager::Get()->RemovePreview(effect);

        delete preview_zone;
    });
}

void OpenRGBEffectPage::on_preview_clicked()
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
    effect->SetFPS(value);
}

void OpenRGBEffectPage::on_Brightness_slider_valueChanged(int value)
{
    effect->SetBrightness(value);
}

void OpenRGBEffectPage::on_RandomCheckbox_clicked()
{
    effect->SetRandomColorsEnabled(ui->RandomCheckbox->isChecked());
}

void OpenRGBEffectPage::on_OnlyFirst_clicked()
{
    effect->SetOnlyFirstColorEnabled(ui->OnlyFirst->isChecked());
}

void OpenRGBEffectPage::on_Temperature_valueChanged(int value)
{
    effect->SetTemperature(value);
}

void OpenRGBEffectPage::on_Tint_valueChanged(int value)
{
    effect->SetTint(value);
}

void OpenRGBEffectPage::SavePatternAction()
{
    std::vector<std::string> filenames = OpenRGBEffectSettings::ListPattern(effect->EffectDetails.EffectClassName);

    QString filename;

    if(filenames.empty())
    {
        filename = QInputDialog::getText(
                    nullptr, "Save pattern to file...", "Choose a filename",
                    QLineEdit::Normal, QString("my-pattern")).trimmed();
    }
    else
    {
        QDialog dialog;

        dialog.setModal(true);
        dialog.setWindowTitle("Save pattern to file...");

        QLabel text1("Choose an existing pattern from this list:", &dialog);
        QLabel text2("Or create a new one:", &dialog);

        QVBoxLayout dialog_layout(&dialog);
        QListWidget list_widget(&dialog);

        for(const std::string& f: filenames)
        {
            list_widget.addItem(QString::fromStdString(f));
        }

        QLineEdit filename_input(&dialog);

        filename_input.setText(QString("my-pattern"));

        dialog_layout.addWidget(&text1);
        dialog_layout.addWidget(&list_widget);
        dialog_layout.addWidget(&text2);
        dialog_layout.addWidget(&filename_input);

        QHBoxLayout buttons_layout;

        QPushButton ok_button;
        ok_button.setText("OK");
        buttons_layout.addWidget(&ok_button);

        QPushButton cancel_button;
        cancel_button.setText("Cancel");
        dialog.connect(&cancel_button,SIGNAL(clicked()),&dialog,SLOT(reject()));
        buttons_layout.addWidget(&cancel_button);

        dialog.connect(&ok_button,SIGNAL(clicked()),&dialog,SLOT(accept()));

        dialog_layout.addLayout(&buttons_layout);

        connect(&list_widget, &QListWidget::currentItemChanged, [&](){
            filename_input.setText(list_widget.currentItem()->text());
        });

        if (dialog.exec())
        {
            filename = filename_input.text();
        }
    }

    if(!filename.isEmpty())
    {
        OpenRGBEffectSettings::SaveEffectPattern(ToJson(), effect->EffectDetails.EffectClassName, filename.toStdString());
    }
}

void OpenRGBEffectPage::LoadPatternAction()
{
    std::vector<std::string> filenames = OpenRGBEffectSettings::ListPattern(effect->EffectDetails.EffectClassName);

    QStringList file_list;

    for(std::string filename : filenames)
    {
        file_list << QString::fromUtf8(filename.c_str());
    }

    if(file_list.empty())
    {
        QMessageBox empty_msg_box(this);
        empty_msg_box.setText("You currently have no patterns for this effect.\nUse the save pattern action first.");
        empty_msg_box.exec();
        return;
    }

    QInputDialog inp(this);

    inp.setOptions(QInputDialog::UseListViewForComboBoxItems);
    inp.setComboBoxItems(file_list);
    inp.setWindowTitle("Load pattern from file...");
    inp.setLabelText("Choose a pattern file from this list:");

    if(!inp.exec()){
        return;
    }

    QString filename = inp.textValue();

    json effect_settings = OpenRGBEffectSettings::LoadPattern(effect->EffectDetails.EffectClassName, filename.toStdString());

    try {
        ApplyJson(effect_settings);
    }  catch (const std::exception & e) {
        printf("[OpenRGBEffectsPlugin] Cannot apply effect settings, reason: %s\n", e.what());
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
    effect_settings["Brightness"] = effect->GetBrightness();
    effect_settings["Temperature"] = effect->GetTemperature();
    effect_settings["Tint"] = effect->GetTint();

    std::vector<RGBColor> colors = effect->GetUserColors();

    for (unsigned int c = 0; c < colors.size(); c++)
    {
        effect_settings["UserColors"][c] = colors[c];
    }

    if (effect->EffectDetails.HasCustomSettings)
    {
        effect_settings["CustomSettings"] = effect->SaveCustomSettings();
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

    if(effect_settings.contains("Brightness"))
    {
        effect->SetBrightness(effect_settings["Brightness"]);
    }

    if(effect_settings.contains("Temperature"))
    {
        effect->SetTemperature(effect_settings["Temperature"]);
    }

    if(effect_settings.contains("Tint"))
    {
        effect->SetTint(effect_settings["Tint"]);
    }

    if(effect_settings.contains("CustomSettings"))
    {
        effect->LoadCustomSettings(effect_settings["CustomSettings"]);
    }

    // Update UI components
    ui->RandomCheckbox->setCheckState(effect->IsRandomColorsEnabled()? Qt::Checked : Qt::Unchecked);
    ui->OnlyFirst->setCheckState(effect->IsOnlyFirstColorEnabled()? Qt::Checked : Qt::Unchecked);
    ui->SpeedSlider->setValue(effect->GetSpeed());
    ui->Slider2->setValue(effect->GetSlider2Val());

    for(unsigned int i = 0; i < ColorPickers.size(); i++)
    {
        ColorPickers[i]->SetRGBColor(colors[i]);
    }
}

void OpenRGBEffectPage::ToggleInfo()
{
    info_visible = !info_visible;

    ui->toggle_info->setText(
                info_visible?
                    OpenRGBPluginsFont::icon(OpenRGBPluginsFont::chevron_up):
                    OpenRGBPluginsFont::icon(OpenRGBPluginsFont::chevron_down)
                    );
    ui->EffectDesciption->setVisible(info_visible);
    ui->time_measure->setVisible(info_visible);
}

void OpenRGBEffectPage::on_toggle_info_clicked()
{
    ToggleInfo();
}

void OpenRGBEffectPage::OpenPatternsFolder()
{
    QUrl url = QUrl::fromLocalFile(QString::fromStdString(OpenRGBEffectSettings::PatternsFolder().string()));
    QDesktopServices::openUrl(url);
}

void OpenRGBEffectPage::EditPatternAction()
{
    QDialog dialog;

    dialog.setModal(true);
    dialog.setWindowTitle("Edit or share your settings.");

    QVBoxLayout dialog_layout(&dialog);

    QTextEdit text_edit(&dialog);

    text_edit.setText(QString::fromStdString(ToJson().dump(4)));

    dialog_layout.addWidget(&text_edit);

    QHBoxLayout buttons_layout;

    QPushButton cancel_button;
    cancel_button.setText("Cancel");
    dialog.connect(&cancel_button,SIGNAL(clicked()),&dialog,SLOT(reject()));
    buttons_layout.addWidget(&cancel_button);

    QPushButton copy_button;
    copy_button.setText("Copy to clipboard");

    dialog.connect(&copy_button, &QPushButton::clicked, [&](){
        std::string text =
                "**" + effect->EffectDetails.EffectClassName + "**" + "\n" +
                "```" +
                text_edit.toPlainText().toStdString() +
                "```" ;

        QGuiApplication::clipboard()->setText(QString::fromStdString(text));
    });

    buttons_layout.addWidget(&copy_button);

    QPushButton accept_button;
    accept_button.setText("Apply");
    dialog.connect(&accept_button,SIGNAL(clicked()),&dialog,SLOT(accept()));
    buttons_layout.addWidget(&accept_button);

    dialog_layout.addLayout(&buttons_layout);

    if (dialog.exec())
    {
        QString text = text_edit.toPlainText();

        if (!text.isEmpty())
        {
            try {
                ApplyJson(json::parse(text.toStdString()));
            } catch (const std::exception & e) {
                printf("[OpenRGBEffectsPlugin] Cannot apply effect settings, reason: %s\n", e.what());
            }
        }
    }
}
