#include "LayerGroupEntry.h"
#include "ui_LayerGroupEntry.h"
#include "ColorUtils.h"

LayerGroupEntry::LayerGroupEntry(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LayerGroupEntry)
{
    ui->setupUi(this);

    QVBoxLayout* outer_layout = new QVBoxLayout(ui->layers);
    layers_layout = new QVBoxLayout(ui->layers);

    outer_layout->addLayout(layers_layout);
    outer_layout->addStretch();

    ui->layers->setLayout(outer_layout);

    for(std::string fn: COLOR_BLEND_FN_NAMES)
    {
        ui->composer_fn->addItem(QString::fromStdString(fn));
    }

    ui->effect_list->ShowStartStopButton(false);
    ui->effect_list->AddEffectsMenus();
}

LayerGroupEntry::~LayerGroupEntry()
{
    for(LayerEntry* layer_entry: layer_entries)
    {
        delete layer_entry;
    }

    delete ui;
}

void LayerGroupEntry::AddLayerEntry(LayerEntry* layer_entry)
{
    connect(layer_entry, &LayerEntry::Remove, [=](){
        layer_entries.erase(std::remove(layer_entries.begin(), layer_entries.end(), layer_entry), layer_entries.end());
        layers_layout->removeWidget(layer_entry);
        delete layer_entry;
    });

    layer_entries.push_back(layer_entry);
    layers_layout->addWidget(layer_entry);

    layer_entry->OnControllerZonesListChanged(assigned_zones);
    layer_entry->EffectState(state);
}

void LayerGroupEntry::StepEffect(std::vector<ControllerZone*> controller_zones, int Brightness, int Temperature, int Tint)
{
    // Reset the current zones to black
    for(ControllerZone* controller_zone: controller_zones)
    {
        controller_zone->SetAllZoneLEDs(ColorUtils::OFF(), 0, 0 , 0);
    }

    // Iterate over groups
    for(unsigned int l = 0; l < layer_entries.size(); l++){

        std::vector<std::vector<RGBColor>> initial_states;

        // Save state
        // No need to save state if only one layer
        if(l > 0)
        {
            for(ControllerZone* controller_zone: controller_zones)
            {
                initial_states.push_back(controller_zone->colors());
            }
        }

        // Run the effect defined by the layer
        layer_entries[l]->StepEffect(controller_zones);

        // Compose + set new state
        // No need to compose if only one layer
        if(l > 0)
        {
            for(unsigned int z = 0; z < controller_zones.size(); z++)
            {
                RGBColor* color_ptr = controller_zones[z]->colors_ptr();

                for(unsigned int i = controller_zones[z]->zone_start_idx(), j = 0; i < controller_zones[z]->zone_stop_idx(); i++, j++)
                {
                    color_ptr[i] = ColorUtils::ApplyColorBlendFn(initial_states[z][j], color_ptr[i], layer_entries[l]->GetComposerFn());
                }
            }
        }
    }

    // Apply global adjustments
    for(ControllerZone* controller_zone: controller_zones)
    {
        RGBColor* current = controller_zone->colors_ptr();

        for(unsigned int i = controller_zone->zone_start_idx(); i < controller_zone->zone_stop_idx(); i ++)
        {
            current[i] = ColorUtils::apply_adjustments(current[i], Brightness / 100.f, Temperature, Tint);
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

void LayerGroupEntry::ClearLayers()
{
    QLayoutItem *child;

    while ((child = layers_layout->takeAt(0)) != 0) {
        delete child->widget();
    }

    layer_entries.clear();
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
            LayerEntry* layer_entry = LayerEntry::FromJson(layer_json);

            if(layer_entry != nullptr)
            {
                AddLayerEntry(layer_entry);
            }
        }
    }

    if(j.contains("composer_fn"))
        ui->composer_fn->setCurrentIndex(j["composer_fn"]);
}

void LayerGroupEntry::on_effect_list_EffectAdded(RGBEffect* effect)
{
    LayerEntry* layer_entry = new LayerEntry(this, effect);
    AddLayerEntry(layer_entry);
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
