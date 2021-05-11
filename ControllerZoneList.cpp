#include "ControllerZoneList.h"
#include "ui_ControllerZoneList.h"

ControllerZoneList::ControllerZoneList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControllerZoneList)
{
    ui->setupUi(this);
}

ControllerZoneList::~ControllerZoneList()
{
    delete ui;
}
