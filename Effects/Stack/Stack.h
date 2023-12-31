#ifndef STACK_H
#define STACK_H

#include <QWidget>
#include "ui_Stack.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"

namespace Ui {
class Stack;
}

class Stack : public RGBEffect
{
    Q_OBJECT

public:
    explicit Stack(QWidget *parent = nullptr);
    ~Stack();

    EFFECT_REGISTERER(ClassName(), CAT_SIMPLE, [](){return new Stack;});

    static std::string const ClassName() {return "Stack";}
    void StepEffect(std::vector<ControllerZone*>) override;
    void OnControllerZonesListChanged(std::vector<ControllerZone*>) override;
    void LoadCustomSettings(json) override;
    json SaveCustomSettings() override;

private slots:
    void on_direction_currentIndexChanged(int);

private:
    Ui::Stack *ui;

    bool reset = false;

    std::vector<unsigned int> stops;
    std::vector<float> progress;
    std::vector<std::vector<RGBColor>> current_colors;
    std::vector<RGBColor> zone_colors;

    RGBColor GetColor(unsigned int, unsigned int);
    void ResetZone(unsigned int, ControllerZone*);
};

#endif // STACK_H
