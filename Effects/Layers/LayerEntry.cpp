#include "LayerEntry.h"
#include "ui_LayerEntry.h"
#include "EffectList.h"
#include "OpenRGBEffectSettings.h"
#include "Layers.h"

LayerEntry::LayerEntry(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LayerEntry)
{
    ui->setupUi(this);

    std::map<std::string, std::function<RGBEffect*()>>::iterator it;

    for (it =  EffectList::effects_construtors.begin(); it !=  EffectList::effects_construtors.end(); it++)
    {
        std::string effect_name = it->first;

        if(effect_name == Layers::ClassName())
        {
            continue;
        }

        std::vector<std::string> patterns = OpenRGBEffectSettings::ListPattern(effect_name);

        if(!patterns.empty())
        {
            ui->effect->addItem(QString::fromStdString(effect_name));

            effects_and_presets[effect_name] = patterns;
        }
    }

    for(std::string fn: COLOR_BLEND_FN_NAMES)
    {
        ui->composer_fn->addItem(QString::fromStdString(fn));
    }
}

LayerEntry::~LayerEntry()
{
    if(effect)
    {
        effect->EffectState(false);
        delete effect;
    }

    delete ui;
}

void LayerEntry::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    if(effect != nullptr)
    {
        mut.lock();
        effect->StepEffect(controller_zones);
        mut.unlock();
    }
}

void LayerEntry::on_effect_currentIndexChanged(int)
{
    std::string effect_name = ui->effect->currentText().toStdString();

    ui->preset->clear();

    for(std::string preset: effects_and_presets[effect_name])
    {
        ui->preset->addItem(QString::fromStdString(preset));
    }
}

void LayerEntry::on_preset_currentIndexChanged(int)
{
    std::string name = ui->effect->currentText().toStdString();
    std::string preset = ui->preset->currentText().toStdString();

    if(name.empty() || preset.empty()){
        return;
    }

    // Re-create effect
    mut.lock();

    if(effect)
    {
        effect->EffectState(false);
        delete effect;
    }

    effect = EffectList::effects_construtors[name]();

    json effect_settings = OpenRGBEffectSettings::LoadPattern(name, preset);

    if(!effect_settings.is_null())
    {
        printf("Applying settings\n");

        std::vector<RGBColor> colors;

        for(unsigned int color : effect_settings["UserColors"])
        {
            colors.push_back(color);
        }

        effect->SetUserColors(colors);
        effect->SetSpeed(effect_settings["Speed"]);
        effect->SetSlider2Val(effect_settings["Slider2Val"]);
        effect->SetRandomColorsEnabled(effect_settings["RandomColors"]);
        effect->SetOnlyFirstColorEnabled(effect_settings["AllowOnlyFirst"]);

        if(effect_settings.contains("CustomSettings"))
        {
            effect->LoadCustomSettings(effect_settings["CustomSettings"]);
        }

        effect->DefineExtraOptions(new QHBoxLayout);
        effect->EffectState(state);
        effect->OnControllerZonesListChanged(assigned_zones);
        printf("Settings applied\n");
    }

    mut.unlock();
}


void LayerEntry::OnControllerZonesListChanged(std::vector<ControllerZone*> controller_zones)
{
    assigned_zones = controller_zones;

    if(effect)
    {
        effect->OnControllerZonesListChanged(controller_zones);
    }
}

void LayerEntry::EffectState(bool state)
{
    this->state = state;

    if(effect)
    {
        effect->EffectState(state);
    }
}

void LayerEntry::on_composer_fn_currentIndexChanged(int fn)
{
    composer_fn = static_cast<ColorBlendFn>(fn);
}

void LayerEntry::on_remove_clicked()
{
    emit Remove();
}

json LayerEntry::ToJson()
{
    json j;

    j["effect"] = ui->effect->currentText().toStdString();
    j["preset"] = ui->preset->currentText().toStdString();
    j["composer_fn"] = ui->composer_fn->currentIndex();

    return j;
}

void LayerEntry::FromJson(json j)
{
    if(j.contains("effect"))
        ui->effect->setCurrentText(QString::fromStdString(j["effect"]));

    if(j.contains("preset"))
        ui->preset->setCurrentText(QString::fromStdString(j["preset"]));

    if(j.contains("composer_fn"))
        ui->composer_fn->setCurrentIndex(j["composer_fn"]);
}
