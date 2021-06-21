#include "OpenRGBEffectTab.h"
#include "OpenRGBEffectPage.h"
#include "OpenRGBEffectSettings.h"
#include "EffectManager.h"
#include "EffectTabHeader.h"
#include "OpenRGBEffectsPlugin.h"
#include "PluginInfo.h"

#include <QTabBar>
#include <QTimer>
#include <QDialog>
#include <QFile>

OpenRGBEffectTab::OpenRGBEffectTab(QWidget *parent):
    QWidget(parent),
    ui(new Ui::OpenRGBEffectTab)
{
    ui->setupUi(this);

    ui->device_list->DisableControls();

    effect_list = new EffectList;

    InitEffectTabs();

    QTimer::singleShot(1000, [=](){
        LoadEffectsFromSettings();
    });
}

OpenRGBEffectTab::~OpenRGBEffectTab()
{
    delete ui;
}

void OpenRGBEffectTab::InitEffectTabs()
{
    QLabel* label = new QLabel("No effects added. Please select one from the list to get started.");
    label->setAlignment(Qt::AlignCenter);

    ui->EffectTabs->insertTab(0, label , "");
    ui->EffectTabs->setTabEnabled(0, false);
    ui->EffectTabs->setCurrentIndex(0);
    ui->EffectTabs->tabBar()->setTabButton(0, QTabBar::RightSide, effect_list);

    SetFirstTabStyle();

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

void OpenRGBEffectTab::SetFirstTabStyle()
{
    ui->EffectTabs->setStyleSheet("QTabBar::tab:only-one, QTabBar::tab:first { border-left: 0; border-right : 1; background: transparent; }");
}

void OpenRGBEffectTab::CreateEffectTab(RGBEffect* effect)
{
    int tab_position = ui->EffectTabs->count();

    EffectTabHeader* effect_header = new EffectTabHeader(nullptr, effect);    
    OpenRGBEffectPage* effect_page = new OpenRGBEffectPage(nullptr, effect);

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

    effect_header->ToogleRunningIndicator(effect->IsAutoStart());

    ui->EffectTabs->setCurrentIndex(tab_position);

    effect_list->ShowStartStopButton(ui->EffectTabs->count() > 1);
}

void OpenRGBEffectTab::DeviceListChanged()
{
    printf("ui->device_list->Clear()\n");
    ui->device_list->Clear();

    if (OpenRGBEffectsPlugin::RMPointer->GetDetectionPercent() < 100)
    {
        return;
    }

    printf("ui->device_list->InitControllersList()\n");
    ui->device_list->InitControllersList();
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

    SetFirstTabStyle();
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

void OpenRGBEffectTab::on_save_settings_clicked()
{
    std::map<RGBEffect*, std::vector<ControllerZone>> effect_zones = EffectManager::Get()->GetEffectsMapping();
    std::map<RGBEffect*, std::vector<ControllerZone>>::iterator it;

    json settings;

    settings["version"] = OpenRGBEffectSettings::version;

    std::vector<json> effects_settings;

    for (it = effect_zones.begin(); it != effect_zones.end(); it++)
    {
        RGBEffect* effect = it->first;
        std::vector<ControllerZone> controller_zones = it->second;

        json effect_settings;

        effect_settings["EffectClassName"] = effect->EffectDetails.EffectClassName;
        effect_settings["FPS"] = effect->GetFPS();
        effect_settings["Speed"] = effect->GetSpeed();
        effect_settings["Slider2Val"] = effect->GetSlider2Val();
        effect_settings["AutoStart"] = effect->IsAutoStart();
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

        std::vector<json> zones;

        for(ControllerZone controller_zone: controller_zones)
        {
            zones.push_back(controller_zone.to_json());
        }

        effect_settings["ControllerZones"] = zones;

        effects_settings.push_back(effect_settings);
    }

    settings["Effects"] = effects_settings;

    OpenRGBEffectSettings::SaveUserSettings(settings);
}

void OpenRGBEffectTab::LoadEffectsFromSettings()
{
    printf("Loading effects settings if any.\n");
    json settings = OpenRGBEffectSettings::LoadUserSettings();

    if(!settings.contains("version") || settings["version"] != OpenRGBEffectSettings::version)
    {
        printf("Trying to load an old settings file version. Aborting.\n");
        return;
    }

    json effects_settings = settings["Effects"];

    for(json effect_settings : effects_settings)
    {
        try {
            LoadEffectSettings(effect_settings);
        }
        catch (const std::exception& e)
        {
            printf("Something went wrong while loading effect: %s.\n", e.what());
        }
        catch(...)
        {
            printf("Unknown error while loading effect.\n");
        }
    }
}

void OpenRGBEffectTab::LoadEffectSettings(json effect_settings)
{
    std::vector<RGBController*> controllers = OpenRGBEffectsPlugin::RMPointer->GetRGBControllers();

    std::string name = effect_settings["EffectClassName"];

    std::vector<RGBColor> colors;

    for(unsigned int color : effect_settings["UserColors"])
    {
        colors.push_back(color);
    }

    std::vector<ControllerZone> controller_zones;

    json zones = effect_settings["ControllerZones"];

    for(auto j : zones)
    {
        for(RGBController* controller: controllers)
        {
            if(
                    controller->name        == j["name"] &&
                    controller->location    == j["location"] &&
                    controller->serial      == j["serial"] &&
                    controller->description == j["description"] &&
                    controller->version     == j["version"] &&
                    controller->vendor      == j["vendor"]
                    )
            {
                controller_zones.push_back(ControllerZone{controller, j["zone_idx"], j["reverse"]});
                break;
            }
        }
    }

    printf("Creating effect. %s\n", name.c_str());

    RGBEffect* effect = EffectList::effects_construtors[name]();

    printf("[%s] Applying basic settings\n", name.c_str());

    effect->SetFPS(effect_settings["FPS"]);
    effect->SetAutoStart(effect_settings["AutoStart"]);
    effect->SetUserColors(colors);
    effect->SetSpeed(effect_settings["Speed"]);
    effect->SetSlider2Val(effect_settings["Slider2Val"]);
    effect->SetRandomColorsEnabled(effect_settings["RandomColors"]);
    effect->SetOnlyFirstColorEnabled(effect_settings["AllowOnlyFirst"]);

    printf("[%s] Loading custom settings\n", name.c_str());

    if(effect_settings.contains("CustomSettings"))
    {
        effect->LoadCustomSettings(effect_settings["CustomSettings"]);
    }

    printf("[%s] Creating effect tab\n", name.c_str());

    CreateEffectTab(effect);

    EffectManager::Get()->Assign(controller_zones, effect);
    ui->device_list->ApplySelection(controller_zones);
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
