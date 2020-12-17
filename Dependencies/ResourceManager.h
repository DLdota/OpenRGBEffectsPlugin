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

#include "Dependencies/i2c_smbus/i2c_smbus.h"
#include "NetworkClient.h"
#include "NetworkServer.h"
#include "ProfileManager.h"
#include "Dependencies/RGBController/RGBController.h"
#include "SettingsManager.h"

#define HID_INTERFACE_ANY   -1
#define HID_USAGE_ANY       -1
#define HID_USAGE_PAGE_ANY  -1L

#define CONTROLLER_LIST_HID 0

struct hid_device_info;

typedef std::function<void(std::vector<i2c_smbus_interface*>&)>                                 I2CBusDetectorFunction;
typedef std::function<void(std::vector<RGBController*>&)>                                       DeviceDetectorFunction;
typedef std::function<void(std::vector<i2c_smbus_interface*>&, std::vector<RGBController*>&)>   I2CDeviceDetectorFunction;
typedef std::function<void(hid_device_info*, const std::string&)>                               HIDDeviceDetectorFunction;
typedef struct
{
    std::string                 name;
    HIDDeviceDetectorFunction   function;
    unsigned int                address;
    int                         interface_no;
    int                         usage_page;
    int                         usage;
} HIDDeviceDetectorBlock;

typedef void (*DeviceListChangeCallback)(void *);
typedef void (*DetectionProgressCallback)(void *);
typedef void (*I2CBusListChangeCallback)(void *);

class ResourceManager
{
public:
    static ResourceManager *get();
    
    ResourceManager();
    virtual ~ResourceManager();
    
    virtual void RegisterI2CBus(i2c_smbus_interface *);
    virtual std::vector<i2c_smbus_interface*> & GetI2CBusses();
    
    virtual void RegisterRGBController(RGBController *rgb_controller);

    virtual std::vector<RGBController*> & GetRGBControllers();
    
    virtual void RegisterI2CBusDetector         (I2CBusDetectorFunction     detector);
    virtual void RegisterDeviceDetector         (std::string name, DeviceDetectorFunction     detector);
    virtual void RegisterI2CDeviceDetector      (std::string name, I2CDeviceDetectorFunction  detector);
    virtual void RegisterHIDDeviceDetector      (std::string name,
                                         HIDDeviceDetectorFunction  detector,
                                         uint16_t vid,
                                         uint16_t pid,
                                         int interface_no = HID_INTERFACE_ANY,
                                         int usage_page   = HID_USAGE_PAGE_ANY,
                                         int usage        = HID_USAGE_ANY);
    
    virtual void RegisterDeviceListChangeCallback(DeviceListChangeCallback new_callback, void * new_callback_arg);
    virtual void RegisterDetectionProgressCallback(DetectionProgressCallback new_callback, void * new_callback_arg);
    virtual void RegisterI2CBusListChangeCallback(I2CBusListChangeCallback new_callback, void * new_callback_arg);

    virtual unsigned int GetDetectionPercent();
    virtual const char*  GetDetectionString();

    virtual std::string                     GetConfigurationDirectory();

    virtual std::vector<NetworkClient*>&    GetClients();
    virtual NetworkServer*                  GetServer();

    virtual ProfileManager*                 GetProfileManager();
    virtual SettingsManager*                GetSettingsManager();

    virtual void                            SetConfigurationDirectory(std::string directory);

    virtual void DeviceListChanged();
    virtual void DetectionProgressChanged();
    virtual void I2CBusListChanged();

    virtual void Cleanup();

    virtual void DetectDevices();

    virtual void DisableDetection();

    virtual void StopDeviceDetection();

    virtual void WaitForDeviceDetection();

private:
    void DetectDevicesThreadFunction();

    static std::unique_ptr<ResourceManager>     instance;

    /*-------------------------------------------------------------------------------------*\
    | Detection enabled flag                                                                |
    \*-------------------------------------------------------------------------------------*/
    bool                                        detection_enabled;
    
    /*-------------------------------------------------------------------------------------*\
    | Profile Manager                                                                       |
    \*-------------------------------------------------------------------------------------*/
    ProfileManager*                             profile_manager;

    /*-------------------------------------------------------------------------------------*\
    | Settings Manager                                                                      |
    \*-------------------------------------------------------------------------------------*/
    SettingsManager*                            settings_manager;

    /*-------------------------------------------------------------------------------------*\
    | I2C/SMBus Interfaces                                                                  |
    \*-------------------------------------------------------------------------------------*/
    std::vector<i2c_smbus_interface*>           busses;

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
    std::vector<I2CBusDetectorFunction>         i2c_bus_detectors;
    std::vector<I2CDeviceDetectorFunction>      i2c_device_detectors;
    std::vector<std::string>                    i2c_device_detector_strings;
    std::vector<HIDDeviceDetectorBlock>         hid_device_detectors;
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
