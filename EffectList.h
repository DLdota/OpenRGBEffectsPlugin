#ifndef EFFECTLIST_H
#define EFFECTLIST_H

#include <set>

#include <QWidget>
#include <QMenu>
#include "RGBEffect.h"
#include "EffectSearch.h"

namespace Ui {
class EffectList;
}

class EffectList : public QWidget
{
    Q_OBJECT

public:
    explicit EffectList(QWidget *parent = nullptr);
    ~EffectList();

    static std::map<std::string, std::function<RGBEffect*()>> effects_construtors;
    static std::map<std::string, std::vector<std::string>> categorized_effects;

    static void RegisterEffect(std::string, std::string, std::function<RGBEffect*()>);
    void ShowStartStopButton(bool);

    void AddMenu(QMenu*);
    void AddAction(QAction*);
    void AddEffectsMenus();

signals:
    void EffectAdded(RGBEffect*);
    void ToggleAllEffectsState();

private slots:
    void on_start_stop_all_button_clicked();
    void AddEffect(std::string);

private:
    Ui::EffectList *ui;
    QMenu* main_menu;
    EffectSearch* effect_search;
    std::vector<QMenu*> sub_menus;
    std::vector<QAction*> sub_actions;
};

#endif // EFFECTLIST_H
