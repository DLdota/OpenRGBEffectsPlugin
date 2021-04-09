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
    if (EffectThreads.find(Effect) == EffectThreads.end())
    {
        ActiveEffects.push_back(Effect);
        EffectThreads[Effect] = nullptr;
        EffectThreads[Effect] = new std::thread(&EffectManager::EffectThreadFunction,this, Effect);
    }
}

void EffectManager::SetEffectUnActive(RGBEffect* Effect)
{    
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

void  EffectManager::EffectThreadFunction(RGBEffect* Effect)
{
    printf("%s thread started\n", Effect->EffectDetails.EffectName.c_str());

    while (EffectThreads.find(Effect) != EffectThreads.end()) {

            TCount start = CLK->now();

            Effect->StepEffect(RespectiveToPass[Effect->EffectDetails.EffectIndex], FPS);

            std::vector<OwnedControllerAndZones> CtrlAndZones = RespectiveToPass[Effect->EffectDetails.EffectIndex];

            for(OwnedControllerAndZones ocz: CtrlAndZones)
            {
                if (ocz.OwnedZones.size() > 0)
                {
                    ocz.Controller->UpdateLEDs();
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

    printf("%s thread ended\n", Effect->EffectDetails.EffectName.c_str());
}
