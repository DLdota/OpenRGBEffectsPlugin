# Contributing guide

We love contributions. Feel free to discuss about a new effect / feature in discord or in the issues.

## Source code

Main directories

```
├── .                   >>> Main program files
├── Dependencies        >>> 3rd party libs and mutualized code
├── Effects             >>> Effects sources
├── OpenRGB             >>> OpenRGB submoduble, needed to build some parts
└──  scripts            >>> CI/CD scripts
```

## Creating a new effect.

* Create a subfolder for your effect inside the `Effects` directory.
* Create a new class inside this new directory, you can use a .h + a .cpp + a .ui file. Dont create a .ui file if you dont need a custom UI.
* The effect class should inherit the RGBEffect class and registers itself.

### Basic example

Header file (h)

```
#ifndef MY_NEW_EFFECT_H
#define MY_NEW_EFFECT_H

#include "RGBEffect.h"
#include "EffectRegisterer.h"

class MyNewEffect: public RGBEffect
{
public:
    MyNewEffect();
    ~MyNewEffect(){}

    EFFECT_REGISTERER(ClassName(), [](){return new MyNewEffect;});
    static std::string const ClassName() { return "MyNewEffect"; }

    void StepEffect(std::vector<ControllerZone>) override;

private:
    float progress = 0.f;
};

#endif // MY_NEW_EFFECT_H
```

Implementation file (cpp)

```
#include "MyNewEffect.h"

REGISTER_EFFECT(MyNewEffect);

MyNewEffect::MyNewEffect() : RGBEffect()
{
    EffectDetails.EffectName = "MyNewEffect";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Some description here";

    EffectDetails.IsReversable = false;
    EffectDetails.MaxSpeed     = 200;
    EffectDetails.MinSpeed     = 40;
    EffectDetails.UserColors   = 1;

    EffectDetails.MaxSlider2Val = 100;
    EffectDetails.MinSlider2Val = 1;
    EffectDetails.Slider2Name   = "Some other control";

    EffectDetails.HasCustomSettings = false;
}

void MyNewEffect::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    for (ControllerZone* controller_zone: controller_zones)
    {
        // use the progress variable to generate a color
        RGBColor myColor = ToRGBColor(255,0,0);
        controller_zone->SetAllZoneLEDs(myColor);
    }

    progress += (float) Speed / (float) FPS;
}
```

Inspect other effects implementation for more details.

### The RGBEffect class

Methods to override :

* DefineExtraOptions: Allows you to manipulate the custom GUI for your effect.
* StepEffect: The engine will call this function for every frames.
* OnControllerZonesListChanged: You get notified for device selection changes
* EffectState: You get notified when the effect state changes (started/stopped)
* LoadCustomSettings: Your effect gets the user settings, time to update the GUI
* SaveCustomSettings: You have to create your own settings json object to be saved.

Protected class members (you can access those values inside your own effect class)

* FPS: frame per second value.
* Speed: speed control value
* Slider2Val: secondary control value
* UserColors: colors choosen by the user
* RandomColorsEnabled: boolean indicating the user checked the random checkbox
* OnlyFirstColorEnabled: boolean indicating the user wants the first color in the color list
* EffectEnabled: boolean indicating the effect state (started/stopped)

## Workflow

Fork this project if you are not a member of OpenRGBDevelopers and clone your fork.

```
git checkout -b my-new-effect
// work ...
git add --all
git commit -m "New effect: EffectName"
git push origin my-new-effect
// Open a merge request.
```

