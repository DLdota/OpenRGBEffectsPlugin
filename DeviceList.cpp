#include "DeviceList.h"
#include "ui_DeviceList.h"
#include "OpenRGBPluginsFont.h"

#include "OpenRGBEffectsPlugin.h"
#include <QVBoxLayout>


DeviceList::DeviceList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceList)
{
    ui->setupUi(this);
    ui->devices->setLayout(new QVBoxLayout(ui->devices));

    ui->toggle_brightness->setFont(OpenRGBPluginsFont::GetFont());
    ui->toggle_brightness->setText(OpenRGBPluginsFont::icon(OpenRGBPluginsFont::sun));

    InitControllersList();
}

DeviceList::~DeviceList()
{
    delete ui;
}

void DeviceList::Clear()
{
    device_items.clear();

    QLayoutItem *child;

    while ((child = ui->devices->layout()->takeAt(0)) != 0)
    {
        delete child->widget();
    }   
}

void DeviceList::InitControllersList()
{
    std::vector<RGBController*> controllers = OpenRGBEffectsPlugin::RMPointer->GetRGBControllers();

    controller_zones.clear();

    for(RGBController* controller : controllers)
    {
        std::vector<ControllerZone*> iteration_zones;

        for(unsigned int i = 0; i < controller->zones.size(); i++)
        {
            ControllerZone* controller_zone = new ControllerZone(controller, i, false, 100, false);
            controller_zones.push_back(controller_zone);
            iteration_zones.push_back(controller_zone);

            for(unsigned int s = 0; s <  controller->zones[i].segments.size(); s++)
            {
                ControllerZone* controller_zone = new ControllerZone(controller, i, false, 100, true, s);
                controller_zones.push_back(controller_zone);
                iteration_zones.push_back(controller_zone);
            }

        }

        if(iteration_zones.empty())
        {
            continue;
        }

        DeviceListItem* item = new DeviceListItem(iteration_zones);
        ui->devices->layout()->addWidget(item);
        device_items.push_back(item);

        connect(item, &DeviceListItem::SelectionChanged, [=](){
            emit SelectionChanged();
        });
    }

    ((QVBoxLayout*) ui->devices->layout())->addStretch(10000);
}

void DeviceList::on_toggle_select_all_clicked()
{
    for(DeviceListItem* item: device_items)
    {
        item->SetEnabled(ui->toggle_select_all->isChecked());
    }

    emit SelectionChanged();
}

void DeviceList::on_toggle_reverse_clicked()
{
    for(DeviceListItem* item: device_items)
    {
        item->SetReverse(ui->toggle_reverse->isChecked());
    }

    emit SelectionChanged();
}

void DeviceList::on_toggle_brightness_clicked()
{
    for(DeviceListItem* item: device_items)
    {
        item->ToggleBrightnessSlider();
    }
}

void DeviceList::DisableControls()
{
    setEnabled(false);

    for(DeviceListItem* item: device_items)
    {
        item->DisableControls();
    }
}

void DeviceList::EnableControls()
{
   setEnabled(true);

    for(DeviceListItem* item: device_items)
    {
        item->EnableControls();
    }
}

std::vector<ControllerZone*> DeviceList::GetControllerZones()
{
    return controller_zones;
}

std::vector<ControllerZone*> DeviceList::GetSelection()
{
    std::vector<ControllerZone*> selection;

    for(DeviceListItem* item: device_items)
    {
        for(ControllerZone* controller_zone: item->GetSelection())
        {
            selection.push_back(controller_zone);
        }
    }

    return selection;
}

void DeviceList::ApplySelection(std::vector<ControllerZone*> selection)
{
    for(DeviceListItem* item: device_items)
    {
        item->ApplySelection(selection);
    }
}
