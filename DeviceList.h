#ifndef DEVICELIST_H
#define DEVICELIST_H

#include <QWidget>
#include "ControllerZone.h"
#include "DeviceListItem.h"

namespace Ui {
class DeviceList;
}

class DeviceList : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceList(QWidget *parent = nullptr);
    ~DeviceList();

    void Clear();
    void InitControllersList();    

    void DisableControls();
    void EnableControls();

    void ApplySelection(std::vector<ControllerZone*>);
    std::vector<ControllerZone*> GetSelection();
    std::vector<ControllerZone*> GetControllerZones();

signals:
    void SelectionChanged();

private slots:
    void on_select_all_clicked();

private:
    Ui::DeviceList *ui;

    std::vector<DeviceListItem*> device_items;
    std::vector<ControllerZone*> controller_zones;

    bool all_selected = false;
};

#endif // DEVICELIST_H
