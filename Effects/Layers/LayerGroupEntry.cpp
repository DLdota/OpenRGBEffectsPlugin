#include "LayerGroupEntry.h"
#include "ui_LayerGroupEntry.h"
#include "ColorUtils.h"

LayerGroupEntry::LayerGroupEntry(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LayerGroupEntry)
{
    ui->setupUi(this);

    for(std::string fn: COLOR_BLEND_FN_NAMES)
    {
        ui->composer_fn->addItem(QString::fromStdString(fn));
    }
}

LayerGroupEntry::~LayerGroupEntry()
{
    for(LayerEntry* layer_entry: layer_entries)
    {
        delete layer_entry;
    }

    delete ui;
}

void LayerGroupEntry::StepEffect(std::vector<ControllerZone*> controller_zones, int Brightness)
{
    for(ControllerZone* controller_zone: controller_zones)
    {
        controller_zone->SetAllZoneLEDs(ColorUtils::OFF(), 0.f);
    }

    for(unsigned int l = 0; l < layer_entries.size(); l++){

        std::vector<std::vector<RGBColor>> previous_states;

        // save state
        if(l > 0)
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

        layer_entries[l]->StepEffect(controller_zones);

        // compose + set new state
        if(l > 0)
        {
            for(unsigned int z = 0; z < controller_zones.size(); z++)
            {
                RGBColor* current = controller_zones[z]->controller->zones[controller_zones[z]->zone_idx].colors;

                for(unsigned int i = 0; i < controller_zones[z]->leds_count(); i ++)
                {
                    current[i] = ColorUtils::ApplyColorBlendFn(previous_states[z][i], current[i], layer_entries[l]->composer_fn);
                }
            }
        }
    }

    // apply global brightness
    for(ControllerZone* controller_zone: controller_zones)
    {
        RGBColor* current = controller_zone->controller->zones[controller_zone->zone_idx].colors;

        for(unsigned int i = 0; i < controller_zone->leds_count(); i ++)
        {
            current[i] = ColorUtils::apply_brightness(current[i], Brightness / 100.f);
        }
    }
}

void LayerGroupEntry::OnControllerZonesListChanged(std::vector<ControllerZone*> controller_zones)
{
    assigned_zones = controller_zones;

    for(LayerEntry* layer_entry: layer_entries)
    {
        layer_entry->OnControllerZonesListChanged(controller_zones);
    }
}

void LayerGroupEntry::EffectState(bool state)
{
    this->state = state;

    for(LayerEntry* layer_entry: layer_entries)
    {
        layer_entry->EffectState(state);
    }
}

LayerEntry* LayerGroupEntry::AddLayerEntry()
{
    LayerEntry* layer_entry = new LayerEntry(this);

    connect(layer_entry, &LayerEntry::Remove, [=](){
        layer_entries.erase(std::remove(layer_entries.begin(), layer_entries.end(), layer_entry), layer_entries.end());
        ui->layers->layout()->removeWidget(layer_entry);
        delete layer_entry;
    });

    layer_entries.push_back(layer_entry);
    ui->layers->layout()->addWidget(layer_entry);

    layer_entry->OnControllerZonesListChanged(assigned_zones);
    layer_entry->EffectState(state);

    return layer_entry;
}

void LayerGroupEntry::ClearLayers()
{
    QLayoutItem *child;

    while ((child = ui->layers->layout()->takeAt(0)) != 0) {
        delete child->widget();
    }

    layer_entries.clear();

    ui->layers->layout()->invalidate();
}

json LayerGroupEntry::ToJson()
{
    json j;

    std::vector<json> layers;

    for(LayerEntry* layer_entry: layer_entries)
    {
        layers.push_back(layer_entry->ToJson());
    }

    j["layers"] = layers;
    j["composer_fn"] = ui->composer_fn->currentIndex();

    return j;
}

void LayerGroupEntry::FromJson(json j)
{
    ClearLayers();

    if(j.contains("layers"))
    {
        for(json layer_json : j["layers"])
        {
            LayerEntry* layer_entry = AddLayerEntry();
            layer_entry->FromJson(layer_json);
        }
    }

    if(j.contains("composer_fn"))
        ui->composer_fn->setCurrentIndex(j["composer_fn"]);
}

void LayerGroupEntry::on_add_layer_clicked()
{
    AddLayerEntry();
}

void LayerGroupEntry::on_clear_clicked()
{
    ClearLayers();
}

void LayerGroupEntry::on_composer_fn_currentIndexChanged(int fn)
{
    composer_fn = static_cast<ColorBlendFn>(fn);
}

void LayerGroupEntry::on_remove_clicked()
{
    emit Remove();
}
