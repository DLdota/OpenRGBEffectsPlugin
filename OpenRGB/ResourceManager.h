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

#include "i2c_smbus.h"
#include "NetworkClient.h"
#include "NetworkServer.h"
#include "ProfileManager.h"
#include "RGBController.h"
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

typedef void (*DeviceListChangeCallback)(void *);
typedef void (*DetectionProgressCallback)(void *);
typedef void (*I2CBusListChangeCallback)(void *);

class ResourceManagerInterface
{
public:
    virtual std::vector<i2c_smbus_interface*> & GetI2CBusses()                                                                                      = 0;

    virtual void                                RegisterRGBController(RGBController *rgb_controller)                                                = 0;

    virtual void                                RegisterDeviceListChangeCallback(DeviceListChangeCallback new_callback, void * new_callback_arg)    = 0;
    virtual void                                RegisterDetectionProgressCallback(DetectionProgressCallback new_callback, void * new_callback_arg)  = 0;
    virtual void                                RegisterI2CBusListChangeCallback(I2CBusListChangeCallback new_callback, void * new_callback_arg)    = 0;

    virtual std::vector<RGBController*> &       GetRGBControllers()                                                                                 = 0;

    virtual std::string                         GetConfigurationDirectory()                                                                         = 0;

    virtual std::vector<NetworkClient*>&        GetClients()                                                                                        = 0;
    virtual NetworkServer*                      GetServer()                                                                                         = 0;

    virtual ProfileManager*                     GetProfileManager()                                                                                 = 0;
    virtual SettingsManager*                    GetSettingsManager()                                                                                = 0;

    virtual void                                DeviceListChanged()                                                                                 = 0;

protected:
    virtual                                    ~ResourceManagerInterface() {};
};

class ResourceManager: public ResourceManagerInterface
{
public:
    static ResourceManager *get();

    ResourceManager();
    ~ResourceManager();

    void RegisterI2CBus(i2c_smbus_interface *);
    std::vector<i2c_smbus_interface*> & GetI2CBusses();

    void RegisterRGBController(RGBController *rgb_controller);

    std::vector<RGBController*> & GetRGBControllers();

    void RegisterI2CBusDetector         (I2CBusDetectorFunction     detector);
    void RegisterDeviceDetector         (std::string name, DeviceDetectorFunction     detector);
    void RegisterI2CDeviceDetector      (std::string name, I2CDeviceDetectorFunction  detector);

    void RegisterDeviceListChangeCallback(DeviceListChangeCallback new_callback, void * new_callback_arg);
    void RegisterDetectionProgressCallback(DetectionProgressCallback new_callback, void * new_callback_arg);
    void RegisterI2CBusListChangeCallback(I2CBusListChangeCallback new_callback, void * new_callback_arg);

    unsigned int GetDetectionPercent();
    const char*  GetDetectionString();

    std::string                     GetConfigurationDirectory();

    std::vector<NetworkClient*>&    GetClients();
    NetworkServer*                  GetServer();

    ProfileManager*                 GetProfileManager();
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
};
