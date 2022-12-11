#include "EffectList.h"
#include "ui_EffectList.h"
#include "ColorUtils.h"
#include "OpenRGBPluginsFont.h"

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

    ui->start_stop_all_button->setFont(OpenRGBPluginsFont::GetFont());
    ui->start_stop_all_button->setText(OpenRGBPluginsFont::icon(OpenRGBPluginsFont::play_track_next_o));
    EnableStartStopButton(false);

    main_menu = new QMenu(this);    
    ui->new_effect->setMenu(main_menu);

    effect_search = new EffectSearch(this);

    QWidgetAction* search_action = new QWidgetAction(this);

    search_action->setDefaultWidget(effect_search);
    main_menu->addAction(search_action);

    connect(effect_search, &EffectSearch::EffectClicked, [=](std::string effect_name){
        main_menu->close();
        AddEffect(effect_name);
    });

    connect(effect_search, &EffectSearch::Searching, [=](bool has_text){
        for(QMenu* menu: sub_menus)
        {
            menu->menuAction()->setVisible(!has_text);
        }
        for(QAction* action: sub_actions)
        {
            action->setVisible(!has_text);
        }
    });

    connect(main_menu, &QMenu::aboutToShow, [=](){
        effect_search->FocusSearch();
    });
}

void EffectList::AddEffectsMenus()
{
    for(auto const& entry: categorized_effects)
    {
        std::string category = entry.first;

        QMenu* category_menu = new QMenu(QString::fromStdString(category), this);
        main_menu->addMenu(category_menu);
        main_menu->setMinimumWidth(188);

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

        sub_menus.push_back(category_menu);
    }
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

void EffectList::EnableStartStopButton(bool state)
{
    ui->start_stop_all_button->setEnabled(state);
}

void EffectList::ShowStartStopButton(bool visible)
{
    ui->start_stop_all_button->setVisible(visible);
}

void EffectList::AddMenu(QMenu* menu)
{
    main_menu->addMenu(menu);
    sub_menus.push_back(menu);
}

void EffectList::AddAction(QAction* action)
{
    main_menu->addAction(action);
    sub_actions.push_back(action);
}
