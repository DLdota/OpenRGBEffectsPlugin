#ifndef EFFECTLIST_H
#define EFFECTLIST_H

#include <QWidget>
#include "RGBEffect.h"

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
    static void RegisterEffect(std::string, std::function<RGBEffect*()>);
    void ShowStartStopButton(bool);

signals:
    void EffectAdded(RGBEffect*);
    void ToggleAllEffectsState();

private slots:
    void on_add_effect_clicked();
    void on_start_stop_all_button_clicked();

private:
    Ui::EffectList *ui;

};

#endif // EFFECTLIST_H
