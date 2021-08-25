#include "LayerEntry.h"
#include "ui_LayerEntry.h"

LayerEntry::LayerEntry(QWidget *parent, std::map<std::string, std::vector<std::string>> effects_and_presets, std::vector<std::string> composer_functions) :
    QWidget(parent),
    ui(new Ui::LayerEntry),
    effects_and_presets(effects_and_presets),
    composer_functions(composer_functions)
{
    ui->setupUi(this);

    std::map<std::string, std::vector<std::string>>::iterator it;

    for (it = effects_and_presets.begin(); it != effects_and_presets.end(); it++)
    {
        std::string effect_name = it->first;
        ui->effect->addItem(QString::fromStdString(effect_name));
    }

    for(std::string fn: composer_functions)
    {
        ui->composer_fn->addItem(QString::fromStdString(fn));
    }
}

LayerEntry::~LayerEntry()
{
    delete ui;
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

    emit PresetUpdated(name, preset);
}

void LayerEntry::on_composer_fn_currentIndexChanged(int value)
{
    emit BlendingUpdated(value);
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
