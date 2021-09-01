#include "Layers.h"
#include "EffectList.h"
#include "OpenRGBEffectSettings.h"
#include "ColorUtils.h"

REGISTER_EFFECT(Layers);

Layers::Layers(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::Layers)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "Layers";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Combine effects together. <br />Only effects that have saved patterns will appear here.<br /> <a href=\"https://en.wikipedia.org/wiki/Blend_modes\">Help about blend modes</a>";

    EffectDetails.IsReversable = false;
    EffectDetails.MaxSpeed     = 0;
    EffectDetails.MinSpeed     = 0;
    EffectDetails.UserColors   = 0;
    EffectDetails.AllowOnlyFirst = false;

    EffectDetails.MaxSlider2Val = 0;
    EffectDetails.MinSlider2Val = 0;
    EffectDetails.Slider2Name   = "";

    EffectDetails.HasCustomWidgets = true;
    EffectDetails.HasCustomSettings = true;

    ui->layer_groups->setLayout(new QVBoxLayout());
    ui->layer_groups->layout()->setSizeConstraint(QLayout::SetFixedSize);
    ui->scrollArea->setWidgetResizable(true);

}

Layers::~Layers()
{
    delete ui;
}

void Layers::DefineExtraOptions(QLayout* layout)
{
    layout->addWidget(this);
}

LayerGroupEntry* Layers::AddLayerGroup()
{
    LayerGroupEntry* layer_group_entry = new LayerGroupEntry(this);

    connect(layer_group_entry, &LayerGroupEntry::Remove, [=](){
        layer_groups_entries.erase(std::remove(layer_groups_entries.begin(), layer_groups_entries.end(), layer_group_entry), layer_groups_entries.end());
        ui->layer_groups->layout()->removeWidget(layer_group_entry);
        delete layer_group_entry;
    });

    ui->layer_groups->layout()->addWidget(layer_group_entry);
    layer_groups_entries.push_back(layer_group_entry);

    layer_group_entry->OnControllerZonesListChanged(assigned_zones);
    layer_group_entry->EffectState(EffectEnabled);

    return layer_group_entry;
}

void Layers::ClearLayerGroups()
{
    QLayoutItem *child;

    while ((child = ui->layer_groups->layout()->takeAt(0)) != 0) {
        delete child->widget();
    }

    layer_groups_entries.clear();
}

void Layers::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    for(unsigned int g = 0; g < layer_groups_entries.size(); g++){

        std::vector<std::vector<RGBColor>> previous_states;

        // save state
        if(g > 0)
        {
            for(ControllerZone* controller_zone: controller_zones)
            {
                std::vector<RGBColor> state;

                RGBColor* current = controller_zone->controller->zones[controller_zone->zone_idx].colors;

                for(unsigned int i = 0; i < controller_zone->leds_count(); i ++)
                {
                    state.push_back(current[i]);
                }

                previous_states.push_back(state);
            }
        }

        // run group
        layer_groups_entries[g]->StepEffect(controller_zones);

        // compose + set new state
        if(g > 0)
        {
            for(unsigned int z = 0; z < controller_zones.size(); z++)
            {
                RGBColor* current = controller_zones[z]->controller->zones[controller_zones[z]->zone_idx].colors;

                for(unsigned int i = 0; i < controller_zones[z]->leds_count(); i ++)
                {
                    current[i] = ColorUtils::ApplyColorBlendFn(previous_states[z][i], current[i], layer_groups_entries[g]->composer_fn);
                }
            }
        }
    }
}

void Layers::on_add_layer_group_clicked()
{
    AddLayerGroup();
}

void Layers::on_clear_clicked()
{
    ClearLayerGroups();
}

void Layers::OnControllerZonesListChanged(std::vector<ControllerZone*> controller_zones)
{
    assigned_zones = controller_zones;

    for(LayerGroupEntry* layer_group_entry: layer_groups_entries)
    {
        layer_group_entry->OnControllerZonesListChanged(controller_zones);
    }
}

void Layers::EffectState(bool state)
{
    EffectEnabled = state;

    for(LayerGroupEntry* layer_group_entry: layer_groups_entries)
    {
        layer_group_entry->EffectState(state);
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

json Layers::SaveCustomSettings(json j)
{
    std::vector<json> groups;

    for(LayerGroupEntry* layer_group_entry: layer_groups_entries)
    {
        groups.push_back(layer_group_entry->ToJson());
    }

    j["groups"] = groups;

    return j;
}
