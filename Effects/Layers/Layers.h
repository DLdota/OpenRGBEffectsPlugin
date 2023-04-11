#ifndef LAYERS_H
#define LAYERS_H

#include <QWidget>
#include "ui_Layers.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"
#include "LayerGroupEntry.h"

namespace Ui {
class Layers;
}

class Layers : public RGBEffect
{
    Q_OBJECT

public:
    explicit Layers(QWidget *parent = nullptr);
    ~Layers();

    EFFECT_REGISTERER(ClassName(), CAT_SPECIAL, [](){return new Layers;});

    static std::string const ClassName() {return "Layers";}
    void StepEffect(std::vector<ControllerZone*>) override;
    void OnControllerZonesListChanged(std::vector<ControllerZone*>) override;
    void EffectState(bool) override;

    void LoadCustomSettings(json) override;
    json SaveCustomSettings() override;


private slots:
    void OnAddGroupClicked();

private:
    Ui::Layers *ui;

    std::vector<ControllerZone*> assigned_zones;
    std::vector<LayerGroupEntry*> layer_groups_entries;

    LayerGroupEntry* AddLayerGroup();
    void ClearLayerGroups();
};

#endif // LAYERS_H
