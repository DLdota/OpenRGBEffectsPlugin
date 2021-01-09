#include "OpenRGBEffectPage.h"

void OpenRGBEffectPage::CreateDeviceSelection(std::string DeviceName)
{
    int NewRow = ui->SelectDevices->rowCount();
    ui->SelectDevices->setRowCount(NewRow + 1);

    QTableWidgetItem* NewItem = new QTableWidgetItem(QString().fromStdString(DeviceName));
    ui->SelectDevices->setItem(NewRow,0,NewItem);

    QCheckBox* SelectedBox = new QCheckBox();
    ui->SelectDevices->setCellWidget(NewRow,1,SelectedBox);


    QCheckBox* ReversedBox = new QCheckBox();
    ui->SelectDevices->setCellWidget(NewRow,2,ReversedBox);

    return;
}

OpenRGBEffectPage::OpenRGBEffectPage(QWidget *parent, RGBEffect* EFCT):
    QWidget(parent),
    ui(new Ui::OpenRGBEffectPage)
{
    ui->setupUi(this);

    ui->SpeedFrame->hide();

    ui->SelectDevices->setColumnCount(3);
    ui->SelectDevices->setHorizontalHeaderLabels({"Device","Enabled","Reversed"});
    std::vector<int> CollumnSizes = {165 , 75, 75};
    for (int i = 0; i < int(CollumnSizes.size()); i++)
    {
        ui->SelectDevices->setColumnWidth(i,CollumnSizes[i]);
    }

    qDebug() << OpenRGBEffectTab::RGBControllerList.size();

    for (int i = 0; i < int(OpenRGBEffectTab::RGBControllerList.size()); i++)
    {
        CreateDeviceSelection(OpenRGBEffectTab::RGBControllerList[i]->name);
    }

    ui->EffectName->setText(QString().fromStdString(EFCT->EffectDetails.EffectName));
    ui->EffectDesciption->setText(QString().fromStdString(EFCT->EffectDetails.EffectDescription));

    if (EFCT->EffectDetails.MinSpeed < EFCT->EffectDetails.MaxSpeed)
    {
        ui->SpeedSlider->setMaximum(EFCT->EffectDetails.MaxSpeed);
        ui->SpeedSlider->setMinimum(EFCT->EffectDetails.MinSpeed);
        ui->SpeedSlider->show();
    }
}

OpenRGBEffectPage::~OpenRGBEffectPage()
{
    delete ui;
}
