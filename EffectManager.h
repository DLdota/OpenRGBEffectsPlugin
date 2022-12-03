#ifndef EFFECTMANAGER_H
#define EFFECTMANAGER_H

#include "ControllerZone.h"
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
    bool IsActive(RGBEffect*);
    void RemoveMapping(RGBEffect*);

    void ClearAssignments();
    void Assign(std::vector<ControllerZone*>, RGBEffect*);
    std::vector<ControllerZone*> GetAssignedZones(RGBEffect*);
    std::map<RGBEffect*, std::vector<ControllerZone*>> GetEffectsMapping();

    bool HasActiveEffects();

    void AddPreview(RGBEffect*, ControllerZone*);
    void RemovePreview(RGBEffect*);


private:
    EffectManager();
    ~EffectManager() {};
    void EffectThreadFunction(RGBEffect*);

    void NotifySelectionChanged(RGBEffect*);

    static EffectManager*   instance;
    std::vector<RGBEffect*> ActiveEffects;   
    std::map<RGBEffect*,std::thread*> EffectThreads;
    std::chrono::steady_clock* clock;

    std::map<RGBEffect*, std::vector<ControllerZone*>> effect_zones;
    std::map<RGBEffect*, ControllerZone*> previews;

    std::mutex lock;
};

#endif // EFFECTMANAGER_H
