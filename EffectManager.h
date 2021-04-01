#ifndef EFFECTMANAGER_H
#define EFFECTMANAGER_H

#include "RGBEffect.h"

/*-----------------------------------------------------------------------------------------------*\
| Define so we have a reasonable variable name instead of having to use auto or the full vaiable  |
\*-----------------------------------------------------------------------------------------------*/
typedef std::chrono::time_point<std::chrono::steady_clock,std::chrono::duration<long long, std::ratio<1,10000000000>>> TCount;

class EffectManager
{
public:    
    static EffectManager* Get();

    void SetEffectActive(RGBEffect*);
    void SetEffectUnActive(RGBEffect*);
    bool CheckReversed(int, int);
    std::vector<OwnedControllerAndZones> GetToPass(int);
    void SetFPS(int);
    void ResetControllers();

    const std::vector<RGBEffect*> EffectList;

    std::vector<std::vector<OwnedControllerAndZones>>    RespectiveToPass;
    std::vector<BetterController>                        Controllers;

private:
    EffectManager();
    ~EffectManager() {};

    static EffectManager*   instance;

    std::vector<RGBEffect*> ActiveEffects;
    int                     FPSDelay;
    int                     FPS;

    std::thread*            StepEffectThread;
    void                    EffectStepTimer();
    std::chrono::steady_clock* CLK;
};

#endif // EFFECTMANAGER_H
