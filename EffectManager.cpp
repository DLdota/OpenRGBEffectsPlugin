#include "EffectManager.h"
#include "ORGBEffectPlugin.h"
#include "Effects.h"

EffectManager* EffectManager::instance;

EffectManager* EffectManager::Get()
{
    if(!instance)
    {
        instance = new EffectManager();
    }

    return instance;
}

EffectManager::EffectManager():
    EffectList({
        new SpectrumCycling,
        new RainbowWave,
        new StarryNight,
        new GradientWave,
        new Breathing,
        new Rain,
        new Ambient,
        new Visor,
        new AudioVisualizer,
        new AudioSync,
        new Wavy,
        new Lightning,
    }),
    CLK(new std::chrono::steady_clock())
{
}

void EffectManager::SetEffectActive(RGBEffect* Effect)
{
    if (std::find(ActiveEffects.begin(), ActiveEffects.end(), Effect) == ActiveEffects.end())
    {
        ActiveEffects.push_back(Effect);
    }

    if(ActiveEffects.size() == 1)
    {
        StepEffectThread = new std::thread(&EffectManager::EffectStepTimer,this);
    }
}

void EffectManager::SetEffectUnActive(RGBEffect* Effect)
{
    std::vector<RGBEffect*>::iterator position = std::find(ActiveEffects.begin(), ActiveEffects.end(), Effect);

    if (position != ActiveEffects.end())
    {
        ActiveEffects.erase(position);
    }

    if(ActiveEffects.size() == 0)
    {
        StepEffectThread->join();
        delete StepEffectThread;
    }
}

void EffectManager::ResetControllers()
{
    /*-----------------------------*\
    | Wipe the list of controllers  |
    \*-----------------------------*/
    Controllers.erase(Controllers.begin(), Controllers.end());

    for (int EffectIndex = 0; EffectIndex < int(EffectList.size()); EffectIndex++)
    {
        RespectiveToPass[EffectIndex].clear();
    }

    /*--------------------------------------------------------*\
    | Grab new controllers and start making entries for them   |
    \*--------------------------------------------------------*/
    std::vector<RGBController*> NewControllers = ORGBPlugin::RMPointer->GetRGBControllers();

    for (int i = 0; i < int(NewControllers.size()); i++)
    {
        bool HasDirectMode = false;
        int  DirectID = 0;
        for (int ModeIndex = 0; ModeIndex < int(NewControllers[i]->modes.size()); ModeIndex++ )
        {
            if (NewControllers[i]->modes[ModeIndex].name == "Direct")
            {
                HasDirectMode = true;
                DirectID   = ModeIndex;
                break;
            }
        }

        BetterController NewItem;
        NewItem.Controller  = NewControllers[i];
        NewItem.Index       = i;
        NewItem.HasDirect   = HasDirectMode;
        NewItem.DirectIndex = DirectID;

        Controllers.push_back(NewItem);

        for (int EffectIndex = 0; EffectIndex < int(EffectList.size()); EffectIndex++)
        {
            OwnedControllerAndZones NewZoneMap;
            NewZoneMap.Controller = NewControllers[i];
            RespectiveToPass[EffectIndex].push_back(NewZoneMap);
        }
    }

}

bool EffectManager::CheckReversed(int DeviceIndex, int ZoneIndex)
{
    return Controllers[DeviceIndex].ReversedZones[ZoneIndex];
}

std::vector<OwnedControllerAndZones> EffectManager::GetToPass(int EffectIndex)
{
    return RespectiveToPass[EffectIndex];
}

void EffectManager::SetFPS(int value)
{
    FPS = value;
    FPSDelay = 1000 / value;

    AudioManager::get()->SetDelay(FPSDelay);
}

void EffectManager::EffectStepTimer()
{
    while (ActiveEffects.size() > 0) {
            TCount start = CLK->now();

            for (int EffectIndex = 0; EffectIndex < int(ActiveEffects.size()); EffectIndex++)
            {
                ActiveEffects[EffectIndex]->StepEffect(RespectiveToPass[ActiveEffects[EffectIndex]->EffectDetails.EffectIndex], FPS);
            }

            // After running through all of the effects proceed to set all of the LEDs on a per zone basis
            for (int ControllerID = 0; ControllerID < int(Controllers.size()); ControllerID++)
            {
                if (Controllers[ControllerID].OwnedZones.size() > 0)
                {
                    Controllers[ControllerID].Controller->UpdateLEDs();
                }
            }

            TCount end = CLK->now();

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
}
