#include "EffectList.h"
#include "ui_EffectList.h"
#include "ColorUtils.h"

std::map<std::string, std::function<RGBEffect*()>> EffectList::effects_construtors = {};

EffectList::EffectList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EffectList)
{
    ui->setupUi(this);
    ui->start_stop_all_button->setVisible(false);

    for(auto const& entry: effects_construtors)
    {
       ui->effects->addItem(QString::fromStdString(entry.first));
    }
}

EffectList::~EffectList()
{
    delete ui;
}

void EffectList::RegisterEffect(std::string class_name, std::function<RGBEffect*()> constructor)
{    
    effects_construtors[class_name] = constructor;
}

void EffectList::on_add_effect_clicked()
{
    QString selected_effect = ui->effects->currentText();
    RGBEffect* effect = effects_construtors[selected_effect.toStdString()]();

    // Add some random colors, so we already see something fancy
    std::vector<RGBColor> random_colors;

    for(unsigned int i = 0; i < effect->EffectDetails.UserColors; i ++)
    {
        random_colors.push_back(ColorUtils::RandomRGBColor());
    }

    effect->SetUserColors(random_colors);

    emit EffectAdded(effect);
}

void EffectList::on_start_stop_all_button_clicked()
{
    emit ToggleAllEffectsState();
}

void EffectList::ShowStartStopButton(bool visible)
{
    ui->start_stop_all_button->setVisible(visible);
}
