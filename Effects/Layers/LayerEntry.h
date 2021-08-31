#ifndef LAYERENTRY_H
#define LAYERENTRY_H

#include <QWidget>
#include "RGBEffect.h"
#include "ColorUtils.h"
#include "json.hpp"

using json = nlohmann::json;

namespace Ui {
class LayerEntry;
}

class LayerEntry : public QWidget
{
    Q_OBJECT

public:
    explicit LayerEntry(QWidget *parent = nullptr);
    ~LayerEntry();

    json ToJson();
    void FromJson(json);

    void StepEffect(std::vector<ControllerZone*>);
    void OnControllerZonesListChanged(std::vector<ControllerZone*>);
    void EffectState(bool);

    RGBEffect* effect = nullptr;
    ColorBlendFn composer_fn = MULTIPLY;

signals:
    void Remove();

private slots:
    void on_effect_currentIndexChanged(int);
    void on_preset_currentIndexChanged(int);
    void on_composer_fn_currentIndexChanged(int);
    void on_remove_clicked();

private:
    Ui::LayerEntry *ui;
    std::map<std::string, std::vector<std::string>> effects_and_presets;

    std::vector<ControllerZone*> assigned_zones;
    bool state = false;

    std::mutex mut;

};

#endif // LAYERENTRY_H