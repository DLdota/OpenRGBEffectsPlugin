#ifndef IRESOURCEMANAGER_H
#define IRESOURCEMANAGER_H

#include "ResourceManager.h"

typedef void (*DeviceListChangeCallback)(void *);
typedef void (*DetectionProgressCallback)(void *);
typedef void (*I2CBusListChangeCallback)(void *);

class IResourceManager{
public:
    virtual std::vector<i2c_smbus_interface*> & GetI2CBusses() = 0;

    virtual void RegisterRGBController(RGBController *rgb_controller) = 0;

    virtual void RegisterDeviceListChangeCallback(DeviceListChangeCallback new_callback, void * new_callback_arg) = 0;
    virtual void RegisterDetectionProgressCallback(DetectionProgressCallback new_callback, void * new_callback_arg) = 0;
    virtual void RegisterI2CBusListChangeCallback(I2CBusListChangeCallback new_callback, void * new_callback_arg) = 0;

    virtual std::vector<RGBController*> & GetRGBControllers() = 0;

    virtual std::string                     GetConfigurationDirectory() = 0;

    virtual std::vector<NetworkClient*>&    GetClients() = 0;
    virtual NetworkServer*                  GetServer() = 0;

    virtual SettingsManager*                GetSettingsManager() = 0;

    virtual void DeviceListChanged() = 0;
protected:
    virtual ~IResourceManager() {};
};
#endif // IRESOURCEMANAGER_H
