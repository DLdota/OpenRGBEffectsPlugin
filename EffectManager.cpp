#include "EffectManager.h"
#include "AudioManager.h"

EffectManager* EffectManager::instance;

EffectManager::EffectManager(): clock(new std::chrono::steady_clock()) {}

EffectManager* EffectManager::Get()
{
    if(!instance)
    {
        instance = new EffectManager();
    }

    return instance;
}

void EffectManager::SetEffectActive(RGBEffect* Effect)
{    
    Effect->EffectState(true);

    if (EffectThreads.find(Effect) == EffectThreads.end())
    {
        ActiveEffects.push_back(Effect);
        EffectThreads[Effect] = nullptr;
        EffectThreads[Effect] = new std::thread(&EffectManager::EffectThreadFunction,this, Effect);
    }
}

void EffectManager::SetEffectUnActive(RGBEffect* Effect)
{
    Effect->EffectState(false);

    if (EffectThreads.find(Effect) != EffectThreads.end())
    {
        std::thread* thread = EffectThreads[Effect];
        EffectThreads.erase(Effect);
        thread->join();
        delete thread;

        std::vector<RGBEffect*>::iterator position = std::find(ActiveEffects.begin(), ActiveEffects.end(), Effect);
        ActiveEffects.erase(position);
    }
}

void EffectManager::RemoveMapping(RGBEffect* effect)
{
    effect_zones.erase(effect);
}

void EffectManager::ClearAssignments()
{
    effect_zones.clear();
}

void EffectManager::Assign(std::vector<ControllerZone> controller_zones, RGBEffect* effect)
{
    printf("Assigning %lu zones to %s\n", controller_zones.size(), effect->EffectDetails.EffectName.c_str());

    effect_zones[effect] = controller_zones;

    // remove from other effects
    std::map<RGBEffect*, std::vector<ControllerZone>>::iterator it;

    for (it = effect_zones.begin(); it != effect_zones.end(); it++)
    {
        RGBEffect* other_effect = it->first;

        if(other_effect == effect)
        {
            continue;
        }

        std::vector<ControllerZone> remaining_zones;
        std::vector<ControllerZone> current_zones = it->second;

        for(ControllerZone& zone : current_zones)
        {
            if(std::find(controller_zones.begin(), controller_zones.end(), zone) == controller_zones.end())
            {
                remaining_zones.push_back(zone);
            }
        }

        effect_zones[other_effect] = remaining_zones;
    }

    // force direct mode
    std::set<RGBController*> controllers;

    for(ControllerZone& controller_zone: controller_zones)
    {
        controllers.insert(controller_zone.controller);
    }

    for(RGBController* controller : controllers)
    {
        for(unsigned int i = 0 ; i < controller->modes.size(); i++)
        {
            if(controller->modes[i].name == "Direct")
            {
                controller->SetMode(i);
                break;
            }
        }
    }

    // notify effect zones has changed
    effect->ASelectionWasChanged(effect_zones[effect]);

}

std::vector<ControllerZone> EffectManager::GetAssignedZones(RGBEffect* effect)
{
    return effect_zones[effect];
}

std::map<RGBEffect*, std::vector<ControllerZone>>EffectManager::GetEffectsMapping()
{
    return effect_zones;
}

void  EffectManager::EffectThreadFunction(RGBEffect* effect)
{
    printf("EFFECT: %s thread started\n", effect->EffectDetails.EffectName.c_str());

    while (EffectThreads.find(effect) != EffectThreads.end()) {

            TCount start = clock->now();

            std::vector<ControllerZone> controller_zones =  effect_zones[effect];

            effect->StepEffect(controller_zones);

            // Use a set to update only once the controllers
            std::set<RGBController*> controllers;

            for(ControllerZone& controller_zone: controller_zones)
            {
                controllers.insert(controller_zone.controller);
            }

            for(RGBController* controller : controllers)
            {
                controller->UpdateLEDs();
            }

            TCount end = clock->now();

            int FPS = effect->GetFPS();
            int FPSDelay = 1000 / FPS;

            int delta = FPSDelay - std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

            if(delta > 0)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(delta));
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }

    printf("EFFECT: %s thread ended\n", effect->EffectDetails.EffectName.c_str());
}

bool EffectManager::HasActiveEffects()
{
    return !ActiveEffects.empty();
}
