#include "LayerEntry.h"
#include "ui_LayerEntry.h"
#include "OpenRGBEffectSettings.h"
#include <QDialog>
#include "EffectList.h"
#include "OpenRGBPluginsFont.h"

LayerEntry::LayerEntry(QWidget *parent, RGBEffect* effect) :
    QWidget(parent),
    ui(new Ui::LayerEntry),
    effect(effect)
{
    ui->setupUi(this);

    ui->edit->setFont(OpenRGBPluginsFont::GetFont());
    ui->remove->setFont(OpenRGBPluginsFont::GetFont());

    ui->edit->setText(OpenRGBPluginsFont::icon(OpenRGBPluginsFont::options));
    ui->remove->setText(OpenRGBPluginsFont::icon(OpenRGBPluginsFont::close));

    PopulateCombos();

    effect_page_dialog = new QDialog(this);
    effect_page = new OpenRGBEffectPage(effect_page_dialog, effect);

    effect_page->SetPreviewButtonVisible(false);
    effect_page->SetFPSSliderVisible(false);

    QVBoxLayout* dialog_layout = new QVBoxLayout(effect_page_dialog);

    dialog_layout->addWidget(effect_page);

    ui->effect_name->setText(QString::fromStdString(effect->EffectDetails.EffectClassName));
}

LayerEntry::~LayerEntry()
{
    effect->EffectState(false);

    effect_page_dialog->hide();

    delete effect;
    delete effect_page;
    delete effect_page_dialog;
    delete ui;
}

void LayerEntry::PopulateCombos()
{
    ui->composer_fn->blockSignals(true);

    for(std::string fn: COLOR_BLEND_FN_NAMES)
    {
        ui->composer_fn->addItem(QString::fromStdString(fn));
    }

    ui->composer_fn->blockSignals(false);
}

void LayerEntry::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    effect->StepEffect(controller_zones);
}

ColorBlendFn LayerEntry::GetComposerFn()
{
    return composer_fn;
}

void LayerEntry::OnControllerZonesListChanged(std::vector<ControllerZone*> controller_zones)
{
    assigned_zones = controller_zones;
    effect->OnControllerZonesListChanged(controller_zones);
}

void LayerEntry::EffectState(bool value)
{
    state = value;
    effect->EffectState(state);
}

void LayerEntry::on_composer_fn_currentIndexChanged(int fn)
{
    composer_fn = static_cast<ColorBlendFn>(fn);
}

void LayerEntry::on_remove_clicked()
{
    emit Remove();
}

void LayerEntry::on_edit_clicked()
{
    effect_page_dialog->setWindowTitle(QString::fromStdString(effect->EffectDetails.EffectName));
    effect_page_dialog->show();
}

json LayerEntry::ToJson()
{
    json j;

    j["effect_settings"]    = effect->ToJson();
    j["composer_fn"]        = ui->composer_fn->currentIndex();

    return j;
}

LayerEntry* LayerEntry::FromJson(json j)
{
    RGBEffect* effect = nullptr;

    if(j.contains("effect_settings"))
    {
        json effect_settings = j["effect_settings"];
        std::string effect_name = effect_settings["EffectClassName"];

        effect = EffectList::effects_construtors[effect_name]();

        if(effect_settings.contains("UserColors"))
        {
            std::vector<RGBColor> colors;

            for(unsigned int color : effect_settings["UserColors"])
            {
                colors.push_back(color);
            }

            effect->SetUserColors(colors);
        }

        effect->SetFPS(effect_settings["FPS"]);
        effect->SetSpeed(effect_settings["Speed"]);
        effect->SetSlider2Val(effect_settings["Slider2Val"]);
        effect->SetRandomColorsEnabled(effect_settings["RandomColors"]);
        effect->SetOnlyFirstColorEnabled(effect_settings["AllowOnlyFirst"]);

        if(effect_settings.contains("Brightness"))
        {
            effect->SetBrightness(effect_settings["Brightness"]);
        }

        if(effect_settings.contains("Temperature"))
        {
            effect->SetTemperature(effect_settings["Temperature"]);
        }

        if(effect_settings.contains("Tint"))
        {
            effect->SetTint(effect_settings["Tint"]);
        }

        if(effect_settings.contains("CustomSettings"))
        {
            effect->LoadCustomSettings(effect_settings["CustomSettings"]);
        }

        LayerEntry* layer_entry = new LayerEntry(nullptr, effect);

        if(j.contains("composer_fn"))
            layer_entry->composer_fn = j["composer_fn"];

        layer_entry->ui->composer_fn->setCurrentIndex(layer_entry->composer_fn);

        return layer_entry;
    }

    return nullptr;
}
