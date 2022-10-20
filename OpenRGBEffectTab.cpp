#include "OpenRGBEffectTab.h"
#include "OpenRGBEffectPage.h"
#include "OpenRGBEffectSettings.h"
#include "EffectManager.h"
#include "EffectTabHeader.h"
#include "OpenRGBEffectsPlugin.h"
#include "PluginInfo.h"
#include "SaveProfilePopup.h"

#include <QAction>
#include <QDialog>
#include <QFile>
#include <QInputDialog>
#include <QTabBar>
#include <QTimer>
#include <QMainWindow>
#include <QMessageBox>

OpenRGBEffectTab::OpenRGBEffectTab(QWidget *parent):
    QWidget(parent),
    ui(new Ui::OpenRGBEffectTab)
{
    ui->setupUi(this);

    ui->device_list->DisableControls();

    effect_list = new EffectList(this);

    InitEffectTabs();

    for (QWidget *w : QApplication::topLevelWidgets())
    {
        if (QMainWindow* mainWin = qobject_cast<QMainWindow*>(w))
        {
            QAction* actionLightsOff = mainWin->findChild<QAction *>("ActionLightsOff");

            if(actionLightsOff)
            {
                connect(actionLightsOff, SIGNAL(triggered()), this, SLOT(OnStopEffects()));
                break;
            }
        }
    }

    // Give a bit to other plugins before loading.
    // So we make sure all virtual devices are ready
    QTimer::singleShot(2000, [=](){
        LoadProfileList();
    });
}

OpenRGBEffectTab::~OpenRGBEffectTab()
{
    delete ui;
}

void OpenRGBEffectTab::LoadProfile(std::string profile)
{
    ui->profiles->setCurrentText(QString::fromStdString(profile));
}

void OpenRGBEffectTab::InitEffectTabs()
{
    QLabel* label = new QLabel("No effects added yet.\n Please select one from the list to get started.");
    label->setAlignment(Qt::AlignCenter);

    ui->EffectTabs->insertTab(0, label , "");
    ui->EffectTabs->setTabEnabled(0, false);
    ui->EffectTabs->setCurrentIndex(0);
    ui->EffectTabs->tabBar()->setTabButton(0, QTabBar::RightSide, effect_list);

    connect(effect_list, &EffectList::EffectAdded, [=](RGBEffect* effect){
        CreateEffectTab(effect);
    });

    connect(effect_list, &EffectList::ToggleAllEffectsState, [=](){
        if(EffectManager::Get()->HasActiveEffects())
        {
            StopAll();
        }
        else
        {
            StartAll();
        }
    });
}

void OpenRGBEffectTab::CreateEffectTab(RGBEffect* effect)
{
    int tab_position = ui->EffectTabs->count();

    EffectTabHeader* effect_header = new EffectTabHeader(ui->EffectTabs->tabBar(), effect);
    OpenRGBEffectPage* effect_page = new OpenRGBEffectPage(this, effect);

    ui->EffectTabs->insertTab(tab_position, effect_page , "");
    ui->EffectTabs->tabBar()->setTabButton(tab_position, QTabBar::RightSide, effect_header);

    connect(effect_page, &OpenRGBEffectPage::EffectState, [=](bool state){
        effect_header->ToogleRunningIndicator(state);
    });

    connect(effect_header, &EffectTabHeader::CloseRequest, [=](){
        int tab_idx = ui->EffectTabs->indexOf(effect_page);
        ui->EffectTabs->removeTab(tab_idx);

        EffectManager::Get()->SetEffectUnActive(effect);
        EffectManager::Get()->RemoveMapping(effect);

        effect_list->ShowStartStopButton(ui->EffectTabs->count() > 1);

        delete effect_page;
        delete effect_header;
        //delete effect;
    });

    connect(effect_header, &EffectTabHeader::Renamed, [=](std::string new_name){
        effect->EffectDetails.CustomName = new_name;
        // stupid hack to trigger resizes
        int tab_idx = ui->EffectTabs->indexOf(effect_page);
        ui->EffectTabs->tabBar()->setTabButton(tab_idx, QTabBar::RightSide, nullptr);
        ui->EffectTabs->tabBar()->setTabButton(tab_idx, QTabBar::RightSide, effect_header);
    });

    connect(effect_header, &EffectTabHeader::StartStopRequest, [=](){
        if(EffectManager::Get()->IsActive(effect))
        {
            effect_page->StopEffect();
        }
        else
        {
            effect_page->StartEffect();
        }
    });

    effect_header->ToogleRunningIndicator(effect->IsAutoStart());

    ui->EffectTabs->setCurrentIndex(tab_position);

    effect_list->ShowStartStopButton(ui->EffectTabs->count() > 1);
}

void OpenRGBEffectTab::DeviceListChanged()
{
    printf("[OpenRGBEffectsPlugin] Clear device list\n");
    ui->device_list->Clear();

    if (OpenRGBEffectsPlugin::RMPointer->GetDetectionPercent() < 100)
    {
        return;
    }

    printf("[OpenRGBEffectsPlugin] Init device list\n");
    ui->device_list->InitControllersList();
}

void OpenRGBEffectTab::LoadProfileList()
{
    std::vector<std::string> profiles = OpenRGBEffectSettings::ListProfiles();
    std::string default_profile = OpenRGBEffectSettings::DefaultProfile();

    ui->profiles->blockSignals(true);

    ui->profiles->clear();

    for (const std::string& file_name: profiles)
    {
        ui->profiles->addItem(QString::fromStdString(file_name));
    }

    if(!default_profile.empty())
    {
        ui->profiles->setCurrentText(QString::fromStdString(default_profile));
    }
    else if(!profiles.empty())
    {
        ui->profiles->setCurrentIndex(0);
    }

    ui->profiles->blockSignals(false);

    LoadEffectsFromCurrentProfile();

    emit ProfileListUpdated();
}

void OpenRGBEffectTab::on_profiles_currentIndexChanged(int)
{
    printf("[OpenRGBEffectsPlugin] Selecting profile: %s\n", ui->profiles->currentText().toStdString().c_str());

    StopAll();
    ClearAll();

    LoadEffectsFromCurrentProfile();
}

void OpenRGBEffectTab::ClearAll()
{
    for(OpenRGBEffectPage* effect_page: ui->EffectTabs->findChildren<OpenRGBEffectPage*>())
    {
        int tab_idx = ui->EffectTabs->indexOf(effect_page);
        ui->EffectTabs->removeTab(tab_idx);
        delete effect_page;
    }

    effect_list->ShowStartStopButton(false);

    EffectManager::Get()->ClearAssignments();
}

void OpenRGBEffectTab::on_EffectTabs_currentChanged(int current)
{
    if(current > 0)
    {
        QWidget* widget = ui->EffectTabs->currentWidget();
        RGBEffect* effect = ((OpenRGBEffectPage*)widget)->GetEffect();

        ui->device_list->EnableControls();
        ui->device_list->ApplySelection(EffectManager::Get()->GetAssignedZones(effect));
    }
    else
    {
        ui->device_list->DisableControls();
        ui->device_list->ApplySelection({});
    }
}

void OpenRGBEffectTab::on_device_list_SelectionChanged()
{
    QWidget* widget = ui->EffectTabs->currentWidget();

    if(dynamic_cast<OpenRGBEffectPage*>(widget) == nullptr)
    {
        return;
    }

    RGBEffect* effect = ((OpenRGBEffectPage*)widget)->GetEffect();

    EffectManager::Get()->Assign(ui->device_list->GetSelection(), effect);
}

void OpenRGBEffectTab::on_plugin_infos_clicked()
{
    QDialog* dialog = new QDialog();
    dialog->setWindowTitle("Effects");

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

    QVBoxLayout* dialog_layout = new QVBoxLayout(dialog);

    PluginInfo* plugin_info = new PluginInfo(dialog);

    dialog_layout->addWidget(plugin_info);

    QHBoxLayout* buttons_layout = new QHBoxLayout();

    QPushButton* close_button = new QPushButton();
    close_button->setText("Close");
    dialog->connect(close_button,SIGNAL(clicked()),dialog,SLOT(reject()));
    buttons_layout->addWidget(close_button);
    dialog_layout->addLayout(buttons_layout);

    dialog->exec();
}

void OpenRGBEffectTab::on_delete_profile_clicked()
{
    QString current_profile = ui->profiles->currentText();

    if(!current_profile.isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setText("Delete profile?");
        msgBox.setInformativeText("Are you sure to want to delete this profile?");
        msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();

        switch (ret) {
        case QMessageBox::Ok:
            if(OpenRGBEffectSettings::DeleteProfile(current_profile.toStdString()))
            {
                ui->profiles->removeItem(ui->profiles->currentIndex());
            }
            break;
        default:
            break;
        }
    }
}

void OpenRGBEffectTab::on_save_settings_clicked()
{
    QString current_text = ui->profiles->currentText();

    if(current_text.isEmpty())
    {
        current_text = "my-profile";
    }

    QDialog* dialog = new QDialog(this);

    dialog->setWindowTitle("Save to profile");

    dialog->setModal(true);

    QVBoxLayout* dialog_layout = new QVBoxLayout(dialog);

    SaveProfilePopup* save_profile_popup = new SaveProfilePopup(dialog);

    save_profile_popup->SetFileName(current_text);

    dialog_layout->addWidget(save_profile_popup);

    connect(save_profile_popup, &SaveProfilePopup::Accept, [=](){

        QString profile_name = save_profile_popup->Filename();
        bool should_load_at_startup = save_profile_popup->ShouldLoadAtStartup();
        bool save_effects_state = save_profile_popup->ShouldSaveEffectsState();

        if (!profile_name.isEmpty())
        {
            std::map<RGBEffect*, std::vector<ControllerZone*>> effect_zones = EffectManager::Get()->GetEffectsMapping();
            std::map<RGBEffect*, std::vector<ControllerZone*>>::iterator it;

            json settings;

            settings["version"] = OpenRGBEffectSettings::version;

            std::vector<json> effects_settings;

            QList<OpenRGBEffectPage*> pages = ui->EffectTabs->findChildren<OpenRGBEffectPage*>();

            for(OpenRGBEffectPage* page: pages)
            {
                RGBEffect* effect = page->GetEffect();
                std::vector<ControllerZone*> controller_zones = effect_zones[effect];

                json effect_settings;

                effect_settings["EffectClassName"] = effect->EffectDetails.EffectClassName;
                effect_settings["CustomName"] = effect->EffectDetails.CustomName;
                effect_settings["FPS"] = effect->GetFPS();
                effect_settings["Speed"] = effect->GetSpeed();
                effect_settings["Slider2Val"] = effect->GetSlider2Val();
                effect_settings["AutoStart"] = save_effects_state ? EffectManager::Get()->IsActive(effect) : false;
                effect_settings["RandomColors"] = effect->IsRandomColorsEnabled();
                effect_settings["AllowOnlyFirst"] = effect->IsOnlyFirstColorEnabled();
                effect_settings["Brightness"] = effect->GetBrightness();

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

                std::vector<json> zones;

                for(ControllerZone* controller_zone: controller_zones)
                {
                    zones.push_back(controller_zone->to_json());
                }

                effect_settings["ControllerZones"] = zones;

                effects_settings.push_back(effect_settings);
            }

            settings["Effects"] = effects_settings;

            OpenRGBEffectSettings::SaveUserProfile(settings, profile_name.toStdString());

            if(ui->profiles->findText(profile_name) == -1){
                ui->profiles->addItem(profile_name);
                ui->profiles->setCurrentText(profile_name);
                emit ProfileListUpdated();
            }

            if(should_load_at_startup)
            {
                OpenRGBEffectSettings::SetDefaultProfile(profile_name.toStdString());
            }
        }

        dialog->accept();
    });

    connect(save_profile_popup, &SaveProfilePopup::Reject, [=](){
        dialog->reject();
    });

    dialog->exec();
}

void OpenRGBEffectTab::LoadEffectsFromCurrentProfile()
{
    QString profile = ui->profiles->currentText();

    printf("[OpenRGBEffectsPlugin] Loading effects settings if any.\n");

    if(!profile.isEmpty())
    {
        json settings = OpenRGBEffectSettings::LoadUserProfile(profile.toStdString());

        if(!settings.contains("version") || settings["version"] != OpenRGBEffectSettings::version)
        {
            printf("[OpenRGBEffectsPlugin] Trying to load an old settings file version. Aborting.\n");
            return;
        }

        json effects_settings = settings["Effects"];

        for(json effect_settings : effects_settings)
        {
            try {
                LoadEffect(effect_settings);
            }
            catch (const std::exception& e)
            {
                printf("[OpenRGBEffectsPlugin] Something went wrong while loading effect: %s.\n", e.what());
            }
            catch(...)
            {
                printf("[OpenRGBEffectsPlugin] Unknown error while loading effect.\n");
            }
        }
    }
}

void OpenRGBEffectTab::LoadEffect(json effect_settings)
{
    std::string name = effect_settings["EffectClassName"];

    std::vector<RGBColor> colors;

    for(unsigned int color : effect_settings["UserColors"])
    {
        colors.push_back(color);
    }

    std::vector<ControllerZone*> saved_zones;

    json zones = effect_settings["ControllerZones"];

    printf("[OpenRGBEffectsPlugin] Looking for corresponding controllers for auto assignement...\n");

    for(auto j : zones)
    {
        bool found = false;

        for(ControllerZone* controller_zone: ui->device_list->GetControllerZones())
        {
            if(
                    controller_zone->controller->name        == j["name"] &&
                    controller_zone->controller->location    == j["location"] &&
                    controller_zone->controller->serial      == j["serial"] &&
                    controller_zone->controller->description == j["description"] &&
                    controller_zone->controller->version     == j["version"] &&
                    controller_zone->controller->vendor      == j["vendor"] &&
                    controller_zone->zone_idx                == j["zone_idx"]
                    )
            {
                controller_zone->reverse = j["reverse"];
                saved_zones.push_back(controller_zone);
                found = true;
                break;
            }
        }

        if(!found)
        {
            std::string dev_name = j["name"];
            std::string dev_location = j["location"];

            printf("[OpenRGBEffectsPlugin] Unable to find device: %s (%s)\n", dev_name.c_str(), dev_location.c_str());
        }
    }

    printf("[OpenRGBEffectsPlugin] Creating effect: %s\n", name.c_str());

    RGBEffect* effect = EffectList::effects_construtors[name]();

    printf("[OpenRGBEffectsPlugin] Effect %s: Applying basic settings\n", name.c_str());

    effect->SetFPS(effect_settings["FPS"]);
    effect->SetAutoStart(effect_settings["AutoStart"]);
    effect->SetUserColors(colors);
    effect->SetSpeed(effect_settings["Speed"]);
    effect->SetSlider2Val(effect_settings["Slider2Val"]);
    effect->SetRandomColorsEnabled(effect_settings["RandomColors"]);
    effect->SetOnlyFirstColorEnabled(effect_settings["AllowOnlyFirst"]);

    if(effect_settings.contains("CustomName"))
    {
        effect->EffectDetails.CustomName = effect_settings["CustomName"];
    }

    if(effect_settings.contains("Brightness"))
    {
        effect->SetBrightness(effect_settings["Brightness"]);
    }
    else
    {
        effect->SetBrightness(100);
    }

    printf("[OpenRGBEffectsPlugin] Effect %s: Loading custom settings\n", name.c_str());

    if(effect_settings.contains("CustomSettings"))
    {
        effect->LoadCustomSettings(effect_settings["CustomSettings"]);
    }

    printf("[OpenRGBEffectsPlugin] Creating effect %s tab\n", name.c_str());

    CreateEffectTab(effect);

    EffectManager::Get()->Assign(saved_zones, effect);
    ui->device_list->ApplySelection(saved_zones);
}


void OpenRGBEffectTab::StartAll()
{
    QList<OpenRGBEffectPage*> pages = ui->EffectTabs->findChildren<OpenRGBEffectPage*>();

    for(OpenRGBEffectPage* page: pages)
    {
        page->StartEffect();
    }
}

void OpenRGBEffectTab::StopAll()
{
    QList<OpenRGBEffectPage*> pages = ui->EffectTabs->findChildren<OpenRGBEffectPage*>();

    for(OpenRGBEffectPage* page: pages)
    {
        page->StopEffect();
    }
}

void OpenRGBEffectTab::OnStopEffects()
{
    StopAll();
}
