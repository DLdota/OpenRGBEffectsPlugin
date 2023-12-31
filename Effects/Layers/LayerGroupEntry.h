#ifndef LAYERGROUPENTRY_H
#define LAYERGROUPENTRY_H

#include <QWidget>
#include "LayerEntry.h"
#include "json.hpp"
#include "ControllerZone.h"

using json = nlohmann::json;

namespace Ui {
class LayerGroupEntry;
}

class LayerGroupEntry : public QWidget
{
    Q_OBJECT

public:
    explicit LayerGroupEntry(QWidget *parent = nullptr);
    ~LayerGroupEntry();
    json ToJson();
    void FromJson(json);

    void StepEffect(std::vector<ControllerZone*>, int, int , int);
    void OnControllerZonesListChanged(std::vector<ControllerZone*>);
    void EffectState(bool);

    ColorBlendFn composer_fn = MULTIPLY;

signals:
    void Remove();

private slots:
    void on_clear_clicked();
    void on_composer_fn_currentIndexChanged(int);
    void on_remove_clicked();
    void on_effect_list_EffectAdded(RGBEffect*);

private:
    Ui::LayerGroupEntry *ui;
    std::vector<LayerEntry*> layer_entries;
    void AddLayerEntry(LayerEntry*);
    void ClearLayers();

    std::vector<ControllerZone*> assigned_zones;
    bool state = false;
    QVBoxLayout* layers_layout;
};

#endif // LAYERGROUPENTRY_H
