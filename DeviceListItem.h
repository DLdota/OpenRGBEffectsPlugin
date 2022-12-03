#ifndef DEVICELISTITEM_H
#define DEVICELISTITEM_H

#include <QWidget>
#include "RGBController.h"
#include "ControllerZone.h"
#include "ZoneListItem.h"

namespace Ui {
class DeviceListItem;
}

class DeviceListItem : public QWidget
{
    Q_OBJECT

public:
    DeviceListItem(std::vector<ControllerZone*>);
    ~DeviceListItem();    
    void SetEnabled(bool);
    void SetReverse(bool);

    void DisableControls();
    void EnableControls();
    void ToggleBrightnessSlider();

    std::vector<ControllerZone*> GetSelection();

    void ApplySelection(std::vector<ControllerZone*>);

signals:
    void SelectionChanged();

private slots:
    void on_enable_toggled(bool);
    void on_reverse_toggled(bool);
    void on_brightness_valueChanged(int);

    void OnZoneListItemEnabled(bool);
    void OnZoneListItemReversed(bool, int);
    void OnZoneListItemBrightnessChanged(int, int);

private:
    Ui::DeviceListItem *ui;
    void SetupZonesListItems();
    std::vector<ZoneListItem*> zone_items;

    void RunGlobalCheckVerification();

    std::vector<ControllerZone*> controller_zones;
    RGBController* controller;
    bool single_zone;
    void UpdateCheckState();
};

#endif // DEVICELISTITEM_H
