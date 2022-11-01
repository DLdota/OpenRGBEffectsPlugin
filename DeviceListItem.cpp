#include "DeviceListItem.h"
#include "ui_DeviceListItem.h"
#include <QVBoxLayout>

DeviceListItem::DeviceListItem(std::vector<ControllerZone*> controller_zones) :
    QWidget(nullptr),
    ui(new Ui::DeviceListItem),
    controller_zones(controller_zones),
    controller(controller_zones.front()->controller)
{
    ui->setupUi(this);
    ui->brightness->setVisible(false);

    bool has_direct = false;

    for(unsigned int i = 0; i < controller->modes.size(); i++)
    {
        if(controller->modes[i].name == "Direct")
        {
            has_direct = true;
            break;
        }
    }

    ui->danger_not_direct->setVisible(!has_direct);

    QString display_name = QString::fromStdString(controller->name);

    ui->device_name->setToolTip(display_name);
    ui->device_name->setText(display_name);

    single_zone = controller_zones.size() == 1;

    if(!single_zone)
    {
        SetupZonesListItems();
    }
}

DeviceListItem::~DeviceListItem()
{
    delete ui;
}

void DeviceListItem::SetupZonesListItems()
{
    for(unsigned int i = 0; i < controller_zones.size(); i++)
    {
        ZoneListItem* item = new ZoneListItem(QString::fromStdString(controller->zones[i].name));
        ui->zones->addWidget(item);
        zone_items.push_back(item);
        item->SetBrightness(controller_zones[i]->self_brightness);

        connect(item, SIGNAL(Enabled(bool)), this, SLOT(OnZoneListItemEnabled(bool)));

        connect(item, &ZoneListItem::Reversed, [=](bool state){
            OnZoneListItemReversed(state, i);
        });

        connect(item, &ZoneListItem::BrightnessChanged, [=](int brightness){
            OnZoneListItemBrightnessChanged(brightness, i);
        });
    }
}

void DeviceListItem::SetEnabled(bool state)
{
    ui->enable->setChecked(state);

    for(ZoneListItem* item:zone_items)
    {
        item->SetEnableChecked(state);
    }
}

void DeviceListItem::on_enable_clicked()
{
    bool state = ui->enable->isChecked();

    for(ZoneListItem* item:zone_items)
    {
        item->SetEnableChecked(state);
    }

    emit SelectionChanged();
}

void DeviceListItem::on_reverse_clicked()
{
    bool state = ui->reverse->isChecked();

    if(single_zone)
    {
        controller_zones[0]->reverse = state;
    }

    for(unsigned int i = 0; i < zone_items.size(); i++)
    {
        zone_items[i]->SetReverseChecked(state);
        controller_zones[i]->reverse = state;
    }

    emit SelectionChanged();
}

void DeviceListItem::on_brightness_valueChanged(int value)
{
    if(single_zone)
    {
        controller_zones[0]->self_brightness = value;
    }
    else
    {
        for(ZoneListItem* item: zone_items)
        {
            item->SetBrightness(value);
        }
    }
}

void DeviceListItem::OnZoneListItemEnabled(bool)
{
    RunGlobalCheckVerification();

    emit SelectionChanged();
}

void DeviceListItem::OnZoneListItemReversed(bool state, int index)
{
    controller_zones[index]->reverse = state;

    RunGlobalCheckVerification();    

    emit SelectionChanged();
}

void DeviceListItem::OnZoneListItemBrightnessChanged(int brightness, int index)
{
    controller_zones[index]->self_brightness = brightness;
}

void DeviceListItem::DisableControls()
{
    ui->enable->setDisabled(true);
    ui->reverse->setDisabled(true);

    for(ZoneListItem* item: zone_items)
    {
        item->DisableControls();
    }
}

void DeviceListItem::EnableControls()
{
    ui->enable->setDisabled(false);
    ui->reverse->setDisabled(false);

    for(ZoneListItem* item: zone_items)
    {
        item->EnableControls();
    }
}

void DeviceListItem::ToggleBrightnessSlider()
{
    ui->brightness->setVisible(!ui->brightness->isVisible());

    for(ZoneListItem* item: zone_items)
    {
        item->ToggleBrightnessSlider();
    }
}

std::vector<ControllerZone*> DeviceListItem::GetSelection()
{
    std::vector<ControllerZone*> selection;

    if(single_zone)
    {
        if(ui->enable->isChecked())
        {
            selection.push_back(controller_zones.front());
        }
    }
    else
    {
        for(unsigned int i = 0; i < zone_items.size(); i++)
        {
            if(zone_items[i]->IsEnabled())
            {
                selection.push_back(controller_zones[i]);
            }
        }
    }

    return selection;
}

void DeviceListItem::ApplySelection(std::vector<ControllerZone*> selection)
{
    // Reset checkboxes
    ui->enable->setChecked(false);
    ui->reverse->setChecked(false);

    for(ZoneListItem* item: zone_items)
    {
        item->SetEnableChecked(false);
        item->SetReverseChecked(false);
    }

    // Set checked if needed
    for(ControllerZone* controller_zone : selection)
    {
        if(controller == controller_zone->controller)
        {
            if(single_zone)
            {
                ui->enable->setChecked(true);
                ui->reverse->setChecked(controller_zone->reverse);
                ui->brightness->setValue(controller_zone->self_brightness);
            }
            else
            {
                zone_items[controller_zone->zone_idx]->SetEnableChecked(true);
                zone_items[controller_zone->zone_idx]->SetReverseChecked(controller_zone->reverse);
                zone_items[controller_zone->zone_idx]->SetBrightness(controller_zone->self_brightness);
            }
        }
    }

    // Reflect sub zone states
    if(!single_zone)
    {
        RunGlobalCheckVerification();
    }
}

void DeviceListItem::RunGlobalCheckVerification()
{
    // if all zones are checked, check the global checkboxes
    // else, uncheck the global checkboxes

    unsigned int enabled_count = 0;
    unsigned int reversed_count = 0;

    for(ZoneListItem* item: zone_items)
    {
        if(item->IsEnabled())
        {
            enabled_count++;
        }

        if(item->IsReversed())
        {
            reversed_count++;
        }
    }

    ui->enable->setChecked(enabled_count == controller->zones.size());
    ui->reverse->setChecked(reversed_count == controller->zones.size());
}
