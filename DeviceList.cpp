#include "DeviceList.h"
#include "ui_DeviceList.h"

#include "OpenRGBEffectsPlugin.h"

DeviceList::DeviceList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceList)
{
    ui->setupUi(this);

    InitControllersList();

    QFont f(ui->enable_label->font());
    f.setPointSize(f.pointSize() * 1.5);
    f.setBold(true);

    ui->enable_label->setFont(f);
    ui->reverse_label->setFont(f);
}

DeviceList::~DeviceList()
{
    delete ui;
}

void DeviceList::Clear()
{
    device_items.clear();
    ui->devices->setRowCount(0);
    ui->devices->clear();
}

void DeviceList::InitControllersList()
{
    std::vector<RGBController*> controllers = OpenRGBEffectsPlugin::RMPointer->GetRGBControllers();

    controller_zones.clear();

    // Hide headers
    ui->devices->horizontalHeader()->hide();
    ui->devices->verticalHeader()->hide();
    ui->devices->setColumnCount(1);
    ui->devices->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->devices->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->devices->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    // Set selection options
    ui->devices->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->devices->setFocusPolicy(Qt::NoFocus);
    ui->devices->setSelectionMode(QAbstractItemView::NoSelection);

    // Set stretch modes
    ui->devices->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->devices->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->devices->setRowCount(controllers.size());

    int row = 0;

    for(RGBController* controller : controllers)
    {
        std::vector<ControllerZone*> iteration_zones;

        for(unsigned int i = 0; i < controller->zones.size(); i++)
        {
            ControllerZone* controller_zone = new ControllerZone(controller, i, false);
            controller_zones.push_back(controller_zone);
            iteration_zones.push_back(controller_zone);
        }

        if(iteration_zones.empty())
        {
            continue;
        }

        DeviceListItem* item = new DeviceListItem(iteration_zones);
        ui->devices->setCellWidget(row++, 0, item);
        device_items.push_back(item);

        connect(item, &DeviceListItem::SelectionChanged, [=](){
            emit SelectionChanged();
        });
    }
}

void DeviceList::on_select_all_clicked()
{
    all_selected = !all_selected;

    ui->select_all->setText(all_selected ? "Deselect all" : "Select all");

    for(DeviceListItem* item: device_items)
    {
        item->SetEnabled(all_selected);
    }

    emit SelectionChanged();
}

void DeviceList::DisableControls()
{
    for(DeviceListItem* item: device_items)
    {
        item->DisableControls();
    }
}

void DeviceList::EnableControls()
{
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
