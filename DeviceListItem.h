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
    DeviceListItem(RGBController*);
    ~DeviceListItem();    
    void SetEnabled(bool);

    void DisableControls();
    void EnableControls();

    std::vector<ControllerZone> GetSelection();

    void ApplySelection(std::vector<ControllerZone>);

signals:
    void SelectionChanged();

private slots:
    void on_enable_clicked();
    void on_reverse_clicked();

    void OnZoneListItemEnabled(bool);
    void OnZoneListItemReversed(bool);

private:
    Ui::DeviceListItem *ui;
    void SetupZonesListItems();
    std::vector<ZoneListItem*> zone_items;

    void RunGlobalCheckVerification();

    RGBController* controller;
    bool single_zone;
};

#endif // DEVICELISTITEM_H
