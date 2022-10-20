#include "EffectList.h"
#include "ui_EffectList.h"
#include "ColorUtils.h"
#include "EffectSearch.h"

#include <QMenu>
#include <QAction>
#include <QWidgetAction>

std::map<std::string, std::function<RGBEffect*()>> EffectList::effects_construtors = {};
std::map<std::string, std::vector<std::string>> EffectList::categorized_effects;

EffectList::EffectList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EffectList)
{
    ui->setupUi(this);

    ShowStartStopButton(false);

    QMenu* new_effect_menu = new QMenu(this);
    ui->new_effect->setMenu(new_effect_menu);

    EffectSearch* effect_search = new EffectSearch(this);
    QWidgetAction* search_action = new QWidgetAction(this);

    search_action->setDefaultWidget(effect_search);
    new_effect_menu->addAction(search_action);

    std::vector<QMenu*> category_menus;

    connect(effect_search, &EffectSearch::EffectClicked, [=](std::string effect_name){
        new_effect_menu->close();
        AddEffect(effect_name);
    });

    for(auto const& entry: categorized_effects)
    {
        std::string category = entry.first;

        QMenu* category_menu = new QMenu(QString::fromStdString(category), this);
        new_effect_menu->addMenu(category_menu);

        std::vector<std::string> effect_names = entry.second;

        std::sort(effect_names.begin(), effect_names.end());

        for(std::string const& effect_name: effect_names)
        {
            QAction* effect_action = new QAction(QString::fromStdString(effect_name), this);
            category_menu->addAction(effect_action);

            connect(effect_action, &QAction::triggered, [=](){
                AddEffect(effect_name);
            });

            effect_search->add(effect_name);
        }

        category_menus.push_back(category_menu);
    }

    connect(effect_search, &EffectSearch::Searching, [=](bool has_text){
        for(const QMenu* menu: category_menus)
        {
            menu->menuAction()->setVisible(!has_text);
        }
    });
}

void EffectList::AddEffect(std::string effect_name)
{
    RGBEffect* effect = effects_construtors[effect_name]();

    // Add some random colors, so we already see something fancy
    std::vector<RGBColor> random_colors;

    for(unsigned int i = 0; i < effect->EffectDetails.UserColors; i ++)
    {
        random_colors.push_back(ColorUtils::RandomRGBColor());
    }

    effect->SetUserColors(random_colors);

    emit EffectAdded(effect);
}

EffectList::~EffectList()
{
    delete ui;
}

void EffectList::RegisterEffect(std::string name, std::string category, std::function<RGBEffect*()> constructor)
{    
    effects_construtors[name] = constructor;

    if (!categorized_effects.count(category))
    {
        std::vector<std::string> effects;
        categorized_effects[category] = effects;
    }

    categorized_effects[category].push_back(name);
}

void EffectList::on_start_stop_all_button_clicked()
{
    emit ToggleAllEffectsState();
}

void EffectList::ShowStartStopButton(bool visible)
{
    ui->start_stop_all_button->setVisible(visible);
}
