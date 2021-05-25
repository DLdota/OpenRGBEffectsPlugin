#include "DeviceListItem.h"
#include "ui_DeviceListItem.h"

DeviceListItem::DeviceListItem(RGBController* controller) :
    QWidget(nullptr),
    ui(new Ui::DeviceListItem)
{
    ui->setupUi(this);

    this->controller = controller;

    bool has_direct = false;

    for(unsigned int i = 0; i <controller->modes.size(); i++)
    {
        if(controller->modes[i].name == "Direct")
        {
            has_direct = true;
            break;
        }
    }

    ui->danger_not_direct->setVisible(!has_direct);

    QFont f = ui->danger_not_direct->font();
    f.setBold(true);
    f.setPointSize(f.pointSize() * 1.5);
    ui->danger_not_direct->setFont(f);

    QString display_name = "";

    if(controller->name.length() > 25)
    {
        display_name = QString::fromStdString(controller->name.substr(0,25) + "...");
        ui->device_name->setToolTip(QString::fromStdString(controller->name));
    }
    else
    {
        display_name = QString::fromStdString(controller->name);
    }

    ui->device_name->setText(display_name);

    single_zone = controller->zones.size() == 1;

    if(single_zone)
    {
        ui->zones->hide();
    }
    else
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
    ui->zones->setRowCount(controller->zones.size());
    ui->zones->setColumnCount(1);

    // Hide headers
    ui->zones->horizontalHeader()->hide();
    ui->zones->verticalHeader()->hide();
    ui->zones->setColumnCount(1);
    ui->zones->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->zones->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->zones->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    // Set selection options
    ui->zones->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->zones->setFocusPolicy(Qt::NoFocus);
    ui->zones->setSelectionMode(QAbstractItemView::NoSelection);

    // Set resize modes
    ui->zones->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->zones->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->zones->setFrameStyle(QFrame::NoFrame);
    ui->zones->setShowGrid(false);

    int final_height = 0;

    for(unsigned int i = 0; i < controller->zones.size(); i++)
    {
        ZoneListItem* item = new ZoneListItem(QString::fromStdString(controller->zones[i].name));
        ui->zones->setCellWidget(i,0, item);
        final_height += item->sizeHint().height() + 2; // include borders
        zone_items.push_back(item);

        connect(item, SIGNAL(Enabled(bool)), this, SLOT(OnZoneListItemEnabled(bool)));
        connect(item, SIGNAL(Reversed(bool)), this, SLOT(OnZoneListItemReversed(bool)));
    }

    ui->zones->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    ui->zones->setMaximumHeight(final_height);
    ui->zones->setFixedHeight(final_height);
}

void DeviceListItem::SetEnabled(bool state)
{
    ui->enable->setCheckState(state ? Qt::Checked : Qt::Unchecked);

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

    for(ZoneListItem* item:zone_items)
    {
        item->SetReverseChecked(state);
    }

    emit SelectionChanged();
}

void DeviceListItem::OnZoneListItemEnabled(bool)
{
    RunGlobalCheckVerification();

    emit SelectionChanged();
}

void DeviceListItem::OnZoneListItemReversed(bool)
{
    RunGlobalCheckVerification();

    emit SelectionChanged();
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

std::vector<ControllerZone> DeviceListItem::GetSelection()
{
    std::vector<ControllerZone> selection;

    if(single_zone)
    {
        if(ui->enable->isChecked())
        {
            selection.push_back(ControllerZone{controller, 0, ui->reverse->isChecked()});
        }
    }
    else
    {
        for(unsigned int i = 0; i < zone_items.size(); i++)
        {
            if(zone_items[i]->IsEnabled())
            {
                selection.push_back(ControllerZone{controller, i, zone_items[i]->IsReversed()});
            }
        }
    }

    return selection;
}

void DeviceListItem::ApplySelection(std::vector<ControllerZone> selection)
{
    // Reset checkboxes
    ui->enable->setCheckState(Qt::Unchecked);
    ui->reverse->setCheckState(Qt::Unchecked);

    for(ZoneListItem* item: zone_items)
    {
        item->SetEnableChecked(false);
        item->SetReverseChecked(false);
    }

    // Set checked if needed
    for(ControllerZone controller_zone : selection)
    {
        if(controller == controller_zone.controller)
        {
            if(single_zone)
            {
                ui->enable->setCheckState(Qt::Checked);
                ui->reverse->setCheckState(controller_zone.reverse ? Qt::Checked : Qt::Unchecked);
            }
            else
            {
                zone_items[controller_zone.zone_idx]->SetEnableChecked(true);
                zone_items[controller_zone.zone_idx]->SetReverseChecked(controller_zone.reverse);
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

    ui->enable->setCheckState(enabled_count == controller->zones.size() ? Qt::Checked : Qt::Unchecked);
    ui->reverse->setCheckState(reversed_count == controller->zones.size() ? Qt::Checked : Qt::Unchecked);
}
