#include "OpenRGBEffectTab.h"
#include "OpenRGBEffectSettings.h"
#include "EffectManager.h"
#include "OpenRGBEffectsPlugin.h"
#include "PluginInfo.h"
#include "SaveProfilePopup.h"
#include "EffectTabHeader.h"
#include "OpenRGBEffectPage.h"
#include "GlobalSettings.h"

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

        std::string startup_profile = OpenRGBEffectSettings::globalSettings.startup_profile;

        if(!startup_profile.empty())
        {
            LoadProfile(QString::fromStdString(startup_profile));
        }
    });   
}

OpenRGBEffectTab::~OpenRGBEffectTab()
{
    delete ui;
}

void OpenRGBEffectTab::InitEffectTabs()
{
    QMenu* manage_profile_menu = new QMenu("Profiles", this);

    load_profile_menu = new QMenu("Load profile", this);
    manage_profile_menu->addMenu(load_profile_menu);

    QAction* save_profile = new QAction("Save", this);
    connect(save_profile, &QAction::triggered, this, &OpenRGBEffectTab::SaveProfileAction);
    manage_profile_menu->addAction(save_profile);

    QAction* delete_profile = new QAction("Delete", this);
    connect(delete_profile, &QAction::triggered, this, &OpenRGBEffectTab::DeleteProfileAction);
    manage_profile_menu->addAction(delete_profile);

    QAction* global_settings = new QAction("Settings", this);
    connect(global_settings, &QAction::triggered, this, &OpenRGBEffectTab::GlobalSettingsAction);

    QAction* plugin_info = new QAction("About", this);
    connect(plugin_info, &QAction::triggered, this, &OpenRGBEffectTab::PluginInfoAction);

    effect_list->AddMenu(manage_profile_menu);    
    effect_list->AddEffectsMenus();
    effect_list->AddAction(global_settings);
    effect_list->AddAction(plugin_info);

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

        effect_list->EnableStartStopButton(ui->EffectTabs->count() > 1);

        delete effect_page;
        delete effect_header;
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

    effect_list->EnableStartStopButton(ui->EffectTabs->count() > 1);
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

    load_profile_menu->clear();

    for (const std::string& file_name: profiles)
    {
        QAction* profile_action = new QAction(QString::fromStdString(file_name), this);

        connect(profile_action, &QAction::triggered,[=](){
            LoadProfile(QString::fromStdString(file_name));
        });

        load_profile_menu->addAction(profile_action);

    } 

    emit ProfileListUpdated();
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

void OpenRGBEffectTab::PluginInfoAction()
{
    QDialog* dialog = new QDialog();
    dialog->setWindowTitle("Effects");
    dialog->setMinimumSize(300,320);
    dialog->setModal(true);

    QVBoxLayout* dialog_layout = new QVBoxLayout(dialog);

    PluginInfo* plugin_info = new PluginInfo(dialog);

    dialog_layout->addWidget(plugin_info);

    dialog->exec();
}

void OpenRGBEffectTab::GlobalSettingsAction()
{
    QDialog dialog;
    dialog.setWindowTitle("Global settings");
    dialog.setModal(true);

    QVBoxLayout dialog_layout(&dialog);

    GlobalSettings global_settings(&dialog);

    dialog_layout.addWidget(&global_settings);

    dialog.exec();
}

void OpenRGBEffectTab::DeleteProfileAction()
{
    QString current_profile = QString::fromStdString(latest_loaded_profile);

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
                LoadProfileList();
            }
            break;
        default:
            break;
        }
    }
}

void OpenRGBEffectTab::SaveProfileAction()
{
    QString current_text = latest_loaded_profile.empty() ? "my-profile" : QString::fromStdString(latest_loaded_profile);

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

            QList<OpenRGBEffectPage*> pages;

            for(int i = 1; i < ui->EffectTabs->count(); i++)
            {
                pages.push_back(dynamic_cast<OpenRGBEffectPage*>(ui->EffectTabs->widget(i)));
            }

            for(OpenRGBEffectPage* page: pages)
            {
                RGBEffect* effect = page->GetEffect();
                std::vector<ControllerZone*> controller_zones = effect_zones[effect];

                json effect_settings = effect->ToJson();

                effect_settings["AutoStart"] = save_effects_state ? EffectManager::Get()->IsActive(effect) : false;

                std::vector<json> zones;

                for(ControllerZone* controller_zone: controller_zones)
                {
                    zones.push_back(controller_zone->to_json());
                }

                effect_settings["ControllerZones"] = zones;

                effects_settings.push_back(effect_settings);
            }

            settings["Effects"] = effects_settings;

            bool ok = OpenRGBEffectSettings::SaveUserProfile(settings, profile_name.toStdString());

            if(!ok)
            {
                QMessageBox::critical(this,"Error","An error has occured, check the application logs for more details.");
            }
            else
            {
                LoadProfileList();

                if(should_load_at_startup)
                {
                    OpenRGBEffectSettings::globalSettings.startup_profile = profile_name.toStdString();
                    OpenRGBEffectSettings::WriteGlobalSettings();
                }
            }
        }

        dialog->accept();
    });

    connect(save_profile_popup, &SaveProfilePopup::Reject, [=](){
        dialog->reject();
    });

    dialog->exec();
}

void OpenRGBEffectTab::LoadProfile(QString profile)
{
    StopAll();
    ClearAll();

    printf("[OpenRGBEffectsPlugin] LoadProfile '%s'.\n", profile.toStdString().c_str());

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

        latest_loaded_profile = profile.toStdString();
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
            bool location_matches = [&]() {
                const auto location_str = j["location"].get<std::string>();
                if (location_str.find("HID: ") == 0)
                {
                    // We don't compare location of hid device since it changes randomly.
                    return true;
                }
                return controller_zone->controller->location == location_str;
            }();

            bool is_segment = j.contains("is_segment") ? (bool)j["is_segment"] : false;
            int segment_idx = j.contains("segment_idx") ? (int)j["segment_idx"] : -1;

            if(
                    location_matches &&
                    controller_zone->controller->name        == j["name"] &&
                    controller_zone->controller->serial      == j["serial"] &&
                    controller_zone->controller->description == j["description"] &&
                    controller_zone->controller->version     == j["version"] &&
                    controller_zone->controller->vendor      == j["vendor"] &&
                    controller_zone->zone_idx                == j["zone_idx"]&&
                    controller_zone->is_segment              == is_segment &&
                    controller_zone->segment_idx             == segment_idx
                    )
            {
                if(j.contains("self_brightness"))
                {
                    controller_zone->self_brightness = j["self_brightness"];
                }
                else
                {
                    controller_zone->self_brightness = 100;
                }

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
    for(int i = 1; i < ui->EffectTabs->count(); i++)
    {
       (dynamic_cast<OpenRGBEffectPage*>(ui->EffectTabs->widget(i)))->StartEffect();
    }
}

void OpenRGBEffectTab::StopAll()
{
    for(int i = 1; i < ui->EffectTabs->count(); i++)
    {
       (dynamic_cast<OpenRGBEffectPage*>(ui->EffectTabs->widget(i)))->StopEffect();
    }
}

void OpenRGBEffectTab::ClearAll()
{
    while(ui->EffectTabs->count() > 1)
    {
        int idx = ui->EffectTabs->count() - 1;

        OpenRGBEffectPage* effect_page = dynamic_cast<OpenRGBEffectPage*>(ui->EffectTabs->widget(idx));
        EffectTabHeader* effect_header = dynamic_cast<EffectTabHeader*>(ui->EffectTabs->tabBar()->tabButton(idx, QTabBar::RightSide));

        ui->EffectTabs->removeTab(idx);

        delete effect_page;
        delete effect_header;
    }

    effect_list->EnableStartStopButton(false);

    EffectManager::Get()->ClearAssignments();
}

void OpenRGBEffectTab::OnStopEffects()
{
    StopAll();
}

void OpenRGBEffectTab::SetEffectState(std::string name, bool running)
{
    QList<OpenRGBEffectPage*> pages = ui->EffectTabs->findChildren<OpenRGBEffectPage*>();

    for(OpenRGBEffectPage* page: pages)
    {
        if (page->GetEffect()->EffectDetails.EffectName == name)
        {
            if (running)
            {
                page->StartEffect();
            } else {
                page->StopEffect();
            }
            break;
        }
    }
}

unsigned char * OpenRGBEffectTab::GetEffectListDescription(unsigned int* data_size)
{
    std::vector<RGBEffect*> registered_effects;
    QList<OpenRGBEffectPage*> pages = ui->EffectTabs->findChildren<OpenRGBEffectPage*>();

    for(OpenRGBEffectPage* page: pages)
    {
        registered_effects.push_back(page->GetEffect());
    }
    unsigned int data_ptr = 0;
    unsigned short num_effects = registered_effects.size();

    *data_size += sizeof(unsigned int);
    *data_size += sizeof(num_effects);

    for (unsigned int i = 0; i < num_effects; i++)
    {
        *data_size += sizeof(unsigned short);
        *data_size += strlen(registered_effects[i]->EffectDetails.EffectName.c_str()) + 1;

        *data_size += sizeof(unsigned short);
        *data_size += strlen(registered_effects[i]->EffectDetails.EffectDescription.c_str()) + 1;

        *data_size += sizeof(bool);
    }

    /*---------------------------------------------------------*\
    | Create data buffer                                        |
    \*---------------------------------------------------------*/
    unsigned char *data_buf = new unsigned char[*data_size];

    /*---------------------------------------------------------*\
    | Copy in num_effects                                       |
    \*---------------------------------------------------------*/
    memcpy(&data_buf[data_ptr], &num_effects, sizeof(num_effects));
    data_ptr += sizeof(num_effects);

    for (unsigned int i = 0; i < num_effects; i++)
    {
        /*---------------------------------------------------------*\
        | Copy in effect name (size+data)                           |
        \*---------------------------------------------------------*/
        unsigned short str_len = strlen(registered_effects[i]->EffectDetails.EffectName.c_str()) + 1;

        memcpy(&data_buf[data_ptr], &str_len, sizeof(unsigned short));
        data_ptr += sizeof(unsigned short);

        strcpy((char *)&data_buf[data_ptr], registered_effects[i]->EffectDetails.EffectName.c_str());
        data_ptr += str_len;

        /*---------------------------------------------------------*\
        | Copy in effect description (size+data)                    |
        \*---------------------------------------------------------*/
        str_len = strlen(registered_effects[i]->EffectDetails.EffectDescription.c_str()) + 1;

        memcpy(&data_buf[data_ptr], &str_len, sizeof(unsigned short));
        data_ptr += sizeof(unsigned short);

        strcpy((char *)&data_buf[data_ptr], registered_effects[i]->EffectDetails.EffectDescription.c_str());
        data_ptr += str_len;

        /*---------------------------------------------------------*\
        | Copy whether effect is enabled (data)                     |
        \*---------------------------------------------------------*/
        bool enabled = registered_effects[i]->IsEnabled();
        memcpy(&data_buf[data_ptr], &enabled, sizeof(bool));
        data_ptr += sizeof(bool);
    }

    return(data_buf);
}
