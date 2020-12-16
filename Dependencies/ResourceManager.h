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

#include "NetworkClient.h"
#include "NetworkServer.h"
#include "RGBController.h"
#include "SettingsManager.h"

#define HID_INTERFACE_ANY   -1
#define HID_USAGE_ANY       -1
#define HID_USAGE_PAGE_ANY  -1L

#define CONTROLLER_LIST_HID 0

struct hid_device_info;

typedef std::function<void(std::vector<RGBController*>&)>                                       DeviceDetectorFunction;

typedef void (*DeviceListChangeCallback)(void *);
typedef void (*DetectionProgressCallback)(void *);
typedef void (*I2CBusListChangeCallback)(void *);

class ResourceManager
{
public:
    static ResourceManager *get();
    
    ResourceManager();
    ~ResourceManager();
    
    void RegisterRGBController(RGBController *rgb_controller);

    std::vector<RGBController*> & GetRGBControllers();

    unsigned int GetDetectionPercent();
    const char*  GetDetectionString();

    std::string                     GetConfigurationDirectory();

    std::vector<NetworkClient*>&    GetClients();
    NetworkServer*                  GetServer();

    SettingsManager*                GetSettingsManager();

    void                            SetConfigurationDirectory(std::string directory);

    void DeviceListChanged();
    void DetectionProgressChanged();
    void I2CBusListChanged();

    void Cleanup();

    void DetectDevices();

    void DisableDetection();

    void StopDeviceDetection();

    void WaitForDeviceDetection();

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
    SettingsManager*                            settings_manager;

    /*-------------------------------------------------------------------------------------*\
    | RGBControllers                                                                        |
    \*-------------------------------------------------------------------------------------*/
    std::vector<RGBController*>                 rgb_controllers_sizes;
    std::vector<RGBController*>                 rgb_controllers_hw;
    std::vector<RGBController*>                 rgb_controllers;

    /*-------------------------------------------------------------------------------------*\
    | Network Server                                                                        |
    \*-------------------------------------------------------------------------------------*/
    NetworkServer*                              server;

    /*-------------------------------------------------------------------------------------*\
    | Network Clients                                                                       |
    \*-------------------------------------------------------------------------------------*/
    std::vector<NetworkClient*>                 clients;

    /*-------------------------------------------------------------------------------------*\
    | Detectors                                                                             |
    \*-------------------------------------------------------------------------------------*/
    std::vector<DeviceDetectorFunction>         device_detectors;
    std::vector<std::string>                    device_detector_strings;
    std::vector<std::string>                    i2c_device_detector_strings;
    std::vector<std::string>                    hid_device_detector_strings;

    /*-------------------------------------------------------------------------------------*\
    | Detection Thread and Detection State                                                  |
    \*-------------------------------------------------------------------------------------*/
    std::thread *                               DetectDevicesThread;
    std::mutex                                  DetectDeviceMutex;

    std::atomic<bool>                           detection_is_required;
    std::atomic<unsigned int>                   detection_percent;
    const char*                                 detection_string;

    /*-------------------------------------------------------------------------------------*\
    | Device List Changed Callback                                                          |
    \*-------------------------------------------------------------------------------------*/
    std::mutex                                  DeviceListChangeMutex;
    std::vector<DeviceListChangeCallback>       DeviceListChangeCallbacks;
    std::vector<void *>                         DeviceListChangeCallbackArgs;

    /*-------------------------------------------------------------------------------------*\
    | Detection Progress Callback                                                           |
    \*-------------------------------------------------------------------------------------*/
    std::mutex                                  DetectionProgressMutex;
    std::vector<DetectionProgressCallback>      DetectionProgressCallbacks;
    std::vector<void *>                         DetectionProgressCallbackArgs;

    /*-------------------------------------------------------------------------------------*\
    | I2C/SMBus Adapter List Changed Callback                                               |
    \*-------------------------------------------------------------------------------------*/
    std::mutex                                  I2CBusListChangeMutex;
    std::vector<I2CBusListChangeCallback>       I2CBusListChangeCallbacks;
    std::vector<void *>                         I2CBusListChangeCallbackArgs;
};
