#include "Layers.h"
#include "OpenRGBEffectSettings.h"
#include "ColorUtils.h"

#include <QToolButton>
#include <QLabel>

REGISTER_EFFECT(Layers);

Layers::Layers(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::Layers)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "Layers";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Combine effects together.<br /> <a href=\"https://en.wikipedia.org/wiki/Blend_modes\">Help about blend modes</a>";
    EffectDetails.HasCustomSettings = true;
    EffectDetails.ExpandCustomSettings = true;
    EffectDetails.SupportsRandom = false;

    // remove intial dummy tabs
    ui->groups->clear();

    ClearLayerGroups();
}

Layers::~Layers()
{
    delete ui;
}

LayerGroupEntry* Layers::AddLayerGroup()
{
    LayerGroupEntry* layer_group_entry = new LayerGroupEntry(this);

    connect(layer_group_entry, &LayerGroupEntry::Remove, [=](){
        layer_groups.erase(std::remove(layer_groups.begin(), layer_groups.end(), layer_group_entry), layer_groups.end());
        delete layer_group_entry;
    });

    ui->groups->addTab(layer_group_entry, QString::fromStdString("Group " + std::to_string(ui->groups->count())));
    layer_groups.push_back(layer_group_entry);

    layer_group_entry->OnControllerZonesListChanged(assigned_zones);
    layer_group_entry->EffectState(EffectEnabled);

    return layer_group_entry;
}

void Layers::ClearLayerGroups()
{
    ui->groups->clear();
    layer_groups.clear();

    // First tab: add button
    QToolButton *new_group_button = new QToolButton();
    new_group_button->setText("New group");

    QLabel* help = new QLabel("Combine multiple effects within a group, and combine groups together");
    help->setWordWrap(true);
    help->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

    ui->groups->addTab(help, QString(""));
    ui->groups->tabBar()->setTabButton(0, QTabBar::RightSide, new_group_button);

    connect(new_group_button, SIGNAL(clicked()), this, SLOT(OnAddGroupClicked()));
}

void Layers::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    // Iterate over groups
    for(unsigned int g = 0; g < layer_groups.size(); g++){

        std::vector<std::vector<RGBColor>> initial_states;

        // Save colors state
        // No need to save the state if we have only one group
        if(g > 0)
        {
            for(ControllerZone* controller_zone: controller_zones)
            {
                initial_states.push_back(controller_zone->colors());
            }
        }

        // Run group effect
        layer_groups[g]->StepEffect(controller_zones, Brightness, Temperature, Tint);

        // Compose + set new state
        // No need to compose if we have only one group
        if(g > 0)
        {
            for(unsigned int z = 0; z < controller_zones.size(); z++)
            {
                RGBColor* color_ptr = controller_zones[z]->colors_ptr();

                for(unsigned int i = controller_zones[z]->zone_start_idx(), j = 0; i < controller_zones[z]->zone_stop_idx(); i++, j++)
                {
                    color_ptr[i] = ColorUtils::ApplyColorBlendFn(initial_states[z][j], color_ptr[i], layer_groups[g]->composer_fn);
                }
            }
        }
    }
}

void Layers::OnAddGroupClicked()
{
    AddLayerGroup();
}

void Layers::OnControllerZonesListChanged(std::vector<ControllerZone*> controller_zones)
{
    assigned_zones = controller_zones;

    for(LayerGroupEntry* layer_group: layer_groups)
    {
        layer_group->OnControllerZonesListChanged(controller_zones);
    }
}

void Layers::EffectState(bool state)
{
    EffectEnabled = state;

    for(LayerGroupEntry* layer_group: layer_groups)
    {
        layer_group->EffectState(state);
    }
}

void Layers::LoadCustomSettings(json j)
{
    ClearLayerGroups();

    if(j.contains("groups"))
    {
        for(json group_json : j["groups"])
        {
            LayerGroupEntry* layer_group_entry = AddLayerGroup();
            layer_group_entry->FromJson(group_json);
        }
    }
}

json Layers::SaveCustomSettings()
{
    json settings;

    std::vector<json> groups;

    for(LayerGroupEntry* layer_group: layer_groups)
    {
        groups.push_back(layer_group->ToJson());
    }

    settings["groups"] = groups;

    return settings;
}
