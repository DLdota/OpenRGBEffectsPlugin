/*-----------------------------------------*\
|  ResourceManager.h                        |
|                                           |
|  OpenRGB Resource Manager controls access |
|  to application components including      |
|  RGBControllers, I2C interfaces, and      |
|  network SDK components                   |
|                                           |
|  Adam Honse (CalcProgrammer1) 9/27/2020   |
\*-----------------------------------------*/

#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <thread>
#include <string>

#include "SettingsManager.h"

#define HID_INTERFACE_ANY   -1
#define HID_USAGE_ANY       -1
#define HID_USAGE_PAGE_ANY  -1L

#define CONTROLLER_LIST_HID 0

struct hid_device_info;

typedef void (*DeviceListChangeCallback)(void *);
typedef void (*DetectionProgressCallback)(void *);
typedef void (*I2CBusListChangeCallback)(void *);

class ResourceManager
{
public:
    static ResourceManager *get();
    
    ResourceManager();
    ~ResourceManager();
    
    unsigned int GetDetectionPercent();
    const char*  GetDetectionString();

    std::string                     GetConfigurationDirectory();

    virtual SettingsManager*        GetSettingsManager();

    void                            SetConfigurationDirectory(std::string directory);

    void DeviceListChanged();
    void DetectionProgressChanged();
    void I2CBusListChanged();

private:
    void DetectDevicesThreadFunction();

    static std::unique_ptr<ResourceManager>     instance;

    /*-------------------------------------------------------------------------------------*\
    | Detection enabled flag                                                                |
    \*-------------------------------------------------------------------------------------*/
    bool                                        detection_enabled;

    /*-------------------------------------------------------------------------------------*\
    | Settings Manager                                                                      |
    \*-------------------------------------------------------------------------------------*/
    const SettingsManager*                      settings_manager;

};
