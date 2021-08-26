#ifndef LAYERS_H
#define LAYERS_H

#include <QWidget>
#include "ui_Layers.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"

#include "LayerEntry.h"

namespace Ui {
class Layers;
}

enum EffectComposerFn
{
    MULTIPLY = 0,
    SCREEN = 1,
    OVERLAY = 2,
    DODGE = 3,
    BURN = 4,
    MASK = 5,
};

class Layer
{

public:
    Layer() {};

    RGBEffect* effect = nullptr;
    EffectComposerFn composer_fn = MULTIPLY;
};

class Layers : public RGBEffect
{
    Q_OBJECT

public:
    explicit Layers(QWidget *parent = nullptr);
    ~Layers();

    EFFECT_REGISTERER(ClassName(), [](){return new Layers;});

    static std::string const ClassName() {return "Layers";}
    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone*>) override;
    void OnControllerZonesListChanged(std::vector<ControllerZone*>) override;
    void EffectState(bool) override;

    void LoadCustomSettings(json) override;
    json SaveCustomSettings(json) override;


private slots:
    void on_add_layer_clicked();
    void on_clear_clicked();

private:
    Ui::Layers *ui;

    std::vector<std::string> function_names = {
        "Multiply",
        "Screen",
        "Overlay",
        "Dodge",
        "Burn",
        "Mask"
    };

    std::vector<Layer*> layers;

    RGBColor ApplyComposerFn(RGBColor, RGBColor, EffectComposerFn);

    LayerEntry* AddLayer();
    void ClearLayers();

    std::vector<ControllerZone*> assigned_zones;

    std::mutex mutex;

};

#endif // LAYERS_H
