#include "EffectList.h"
#include "ui_EffectList.h"

#include "SpectrumCycling.h"
#include "RainbowWave.h"
#include "StarryNight.h"
#include "GradientWave.h"
#include "Breathing.h"
#include "Rain.h"
#include "Ambient.h"
#include "Visor.h"
#include "AudioVisualizer.h"
#include "AudioSync.h"
#include "Wavy.h"
#include "Lightning.h"
#include "Bubbles.h"

std::map<std::string, std::function<RGBEffect*()>> EffectList::effects_construtors = {
    {SpectrumCycling::ClassName(), [](){return new SpectrumCycling;}},
    {RainbowWave::ClassName(),     [](){return new RainbowWave;}},
    {StarryNight::ClassName(),     [](){return new StarryNight;}},
    {GradientWave::ClassName(),    [](){return new GradientWave;}},
    {Breathing::ClassName(),       [](){return new Breathing;}},
    {Rain::ClassName(),            [](){return new Rain;}},
    {Ambient::ClassName(),         [](){return new Ambient;}},
    {Visor::ClassName(),           [](){return new Visor;}},
    {AudioVisualizer::ClassName(), [](){return new AudioVisualizer;}},
    {AudioSync::ClassName(),       [](){return new AudioSync;}},
    {Wavy::ClassName(),            [](){return new Wavy;}},
    {Lightning::ClassName(),       [](){return new Lightning;}},
    {Bubbles::ClassName(),         [](){return new Bubbles;}}
};

EffectList::EffectList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EffectList)
{
    ui->setupUi(this);

    for(auto const& entry: effects_construtors)
    {
       ui->effects->addItem(QString::fromStdString(entry.first));
    }
}

EffectList::~EffectList()
{
    delete ui;
}

void EffectList::on_add_effect_clicked()
{
    QString selected_effect = ui->effects->currentText();
    RGBEffect* effect = effects_construtors[selected_effect.toStdString()]();
    emit EffectAdded(effect);
}
