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
    EffectDetails.EffectDescription = "Combine effect. \nOnly effects that have saved patterns will appear here.";

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

    ui->layers->setLayout(new QVBoxLayout());
    ui->layers->layout()->setSizeConstraint(QLayout::SetFixedSize);
    ui->scrollArea->setWidgetResizable(true);

    printf("%d\n",ToRGBColor(0,0,255));
}

Layers::~Layers()
{
    delete ui;
}

void Layers::DefineExtraOptions(QLayout* layout)
{
    layout->addWidget(this);
}

void Layers::StepEffect(std::vector<ControllerZone*> controller_zones)
{    
    mutex.lock();

    for(unsigned int l = 0; l < layers.size(); l++){

        RGBEffect* effect = layers[l]->effect;

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

        // run effect
        if(effect != nullptr)
        {
            effect->StepEffect(controller_zones);
        }

        // compose + set new state
        if(l > 0)
        {
            for(unsigned int z = 0; z < controller_zones.size(); z++)
            {
                RGBColor* current = controller_zones[z]->controller->zones[controller_zones[z]->zone_idx].colors;

                for(unsigned int i = 0; i < controller_zones[z]->leds_count(); i ++)
                {
                    current[i] = ApplyComposerFn(previous_states[z][i], current[i], layers[l]->composer_fn);
                }
            }
        }

    }

    mutex.unlock();
}

RGBColor Layers::ApplyComposerFn(RGBColor c1, RGBColor c2, EffectComposerFn fn)
{
    switch (fn)
    {
    case MULTIPLY: return ColorUtils::Multiply(c1, c2);
    case SCREEN:   return ColorUtils::Screen(c1, c2);
    case OVERLAY:  return ColorUtils::Overlay(c1, c2);
    case DODGE:    return ColorUtils::Dodge(c1, c2);
    case BURN:     return ColorUtils::Burn(c1, c2);
    case MASK:     return ColorUtils::Mask(c1, c2);
    default:       return ColorUtils::OFF();
    }
}

LayerEntry* Layers::AddLayer()
{
    std::map<std::string, std::vector<std::string>> effects_and_presets;

    std::map<std::string, std::function<RGBEffect*()>>::iterator it;

    for (it =  EffectList::effects_construtors.begin(); it !=  EffectList::effects_construtors.end(); it++)
    {
        std::vector<std::string> patterns = OpenRGBEffectSettings::ListPattern(it->first);

        if(!patterns.empty() && it->first != ClassName())
        {
            effects_and_presets[it->first] = patterns;
        }
    }

    LayerEntry* layer_ui = new LayerEntry(this, effects_and_presets, function_names);

    Layer* layer = new Layer();

    layers.push_back(layer);

     connect(layer_ui, &LayerEntry::BlendingUpdated, [=](int fn){
          layer->composer_fn = static_cast<EffectComposerFn>(fn);
     });

    connect(layer_ui, &LayerEntry::PresetUpdated, [=](std::string name, std::string preset){

        mutex.lock();

        printf("PresetUpdated\n");

        if(layer->effect != nullptr)
        {
            delete layer->effect;
        }

        layer->effect = EffectList::effects_construtors[name]();

        json effect_settings = OpenRGBEffectSettings::LoadPattern(name, preset);

        if(!effect_settings.is_null())
        {
            printf("Applying settings\n");

            std::vector<RGBColor> colors;

            for(unsigned int color : effect_settings["UserColors"])
            {
                colors.push_back(color);
            }

            layer->effect->SetUserColors(colors);
            layer->effect->SetSpeed(effect_settings["Speed"]);
            layer->effect->SetSlider2Val(effect_settings["Slider2Val"]);
            layer->effect->SetRandomColorsEnabled(effect_settings["RandomColors"]);
            layer->effect->SetOnlyFirstColorEnabled(effect_settings["AllowOnlyFirst"]);

            if(effect_settings.contains("CustomSettings"))
            {
                layer->effect->LoadCustomSettings(effect_settings["CustomSettings"]);
            }

            layer->effect->DefineExtraOptions(new QHBoxLayout);
            layer->effect->OnControllerZonesListChanged(assigned_zones);
            layer->effect->EffectState(EffectEnabled);
            printf("Settings applied\n");
        }

        mutex.unlock();

    });

    connect(layer_ui, &LayerEntry::Remove, [=](){

        mutex.lock();

        if(layer->effect != nullptr)
        {
            layer->effect->EffectState(false);
            delete layer->effect;
        }

        layers.erase(std::remove(layers.begin(), layers.end(), layer), layers.end());
        ui->layers->layout()->removeWidget(layer_ui);
        delete layer_ui;

        mutex.unlock();
    });

    ui->layers->layout()->addWidget(layer_ui);

    return layer_ui;
}

void Layers::ClearLayers()
{
    QLayoutItem *child;

    while ((child = ui->layers->layout()->takeAt(0)) != 0) {
        delete child->widget();
    }

    layers.clear();
}


void Layers::on_add_layer_clicked()
{
    AddLayer();
}

void Layers::on_clear_clicked()
{
    ClearLayers();
}


void Layers::OnControllerZonesListChanged(std::vector<ControllerZone*> controller_zones)
{
    assigned_zones = controller_zones;

    for(Layer* layer: layers)
    {
        if(layer->effect != nullptr)
        {
            layer->effect->OnControllerZonesListChanged(controller_zones);
        }
    }
}

void Layers::EffectState(bool state)
{
    EffectEnabled = state;

    for(Layer* layer: layers)
    {
        if(layer->effect != nullptr)
        {
            layer->effect->EffectState(state);
        }
    }
}

void Layers::LoadCustomSettings(json j)
{
    ClearLayers();

    if(j.contains("layers"))
    {
        for(json layer_json : j["layers"])
        {
            LayerEntry* layer_ui = AddLayer();
            layer_ui->FromJson(layer_json);
        }
    }
}

json Layers::SaveCustomSettings(json j)
{
    std::vector<json> layers_json;

    for(LayerEntry* layer_ui: ui->layers->findChildren<LayerEntry*>())
    {
        layers_json.push_back(layer_ui->ToJson());
    }

    j["layers"] = layers_json;

    return j;
}
