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

    OpenRGBEffectPage::EFCT = EFCT;

    ui->SpeedFrame->hide();
    ui->StopButton->setDisabled(true);


    ui->SelectDevices->setMinimumWidth(317);
    ui->SelectDevices->setColumnCount(3);
    ui->SelectDevices->setHorizontalHeaderLabels({"Device","Enabled","Reversed"});
    /*-------------------*\
    | Set collumn sizes   |
    \*-------------------*/
    std::vector<int> CollumnSizes = {165 , 75, 75};
    for (int i = 0; i < int(CollumnSizes.size()); i++)
    {
        ui->SelectDevices->setColumnWidth(i,CollumnSizes[i]);
    }

    /*---------------------------------*\
    | Fill in top description and name  |
    \*---------------------------------*/
    ui->EffectName->setText(QString().fromStdString(EFCT->EffectDetails.EffectName));
    ui->EffectDesciption->setText(QString().fromStdString(EFCT->EffectDetails.EffectDescription));

    /*-----------------------------------------------*\
    | If maximum speed is greater than Minimum speed  |
    | Set start and stop of speed slider              |
    \*-----------------------------------------------*/
    if (EFCT->EffectDetails.MinSpeed < EFCT->EffectDetails.MaxSpeed)
    {
        ui->SpeedSlider->setMaximum(EFCT->EffectDetails.MaxSpeed);
        ui->SpeedSlider->setMinimum(EFCT->EffectDetails.MinSpeed);
        ui->SpeedFrame->show();
        OpenRGBEffectPage::HasSpeed = true;
    }

    ORGBPlugin::RMPointer->RegisterDeviceListChangeCallback(DeviceListChangedCallback, this);
    ORGBPlugin::RMPointer->RegisterDetectionProgressCallback(DeviceListChangedCallback, this);
}

OpenRGBEffectPage::~OpenRGBEffectPage()
{
    delete ui;
}

void OpenRGBEffectPage::on_StartButton_clicked()
{
    ui->StartButton->setDisabled(true);
    ui->StopButton->setDisabled(false);
    ui->RunningStatus->setText("Running");

    std::vector<RGBController*> RequestLock;

    for (int i = 0; i < ui->SelectDevices->rowCount(); i++)
    {
        /*-----------------------------------------------------*\
        | For row in table widget                               |
        |   If checkbox is checked                              |
        |       Add the Device to the list that will be locked  |
        \*-----------------------------------------------------*/
        QCheckBox *Selectedbox = qobject_cast<QCheckBox *>(ui->SelectDevices->cellWidget(i,1));
        if (Selectedbox->isChecked())
        {
            //qDebug() << QString().fromStdString(ORGBPlugin::RMPointer->GetRGBControllers()[i]->name);
            RequestLock.push_back(ORGBPlugin::RMPointer->GetRGBControllers()[i]);
        }
    }
    OpenRGBEffectPage::OwnedController = OpenRGBEffectTab::LockControllers(RequestLock);
    if (OpenRGBEffectPage::OwnedController.size() > 0)
    {
        // Here is where it SHOULD be running the effect in a different thread
        //std::thread EffectThread(OpenRGBEffectPage::EFCT->StartEffect());
        std::thread EffectThread(&::RGBEffect::StartEffect, EFCT);
    }
}

void OpenRGBEffectPage::on_StopButton_clicked()
{
    OpenRGBEffectPage::EFCT->StopEffect();
    ui->StartButton->setDisabled(false);
    ui->StopButton->setDisabled(true);
    ui->RunningStatus->setText("Stopped");
}

void OpenRGBEffectPage::on_SpeedSlider_valueChanged(int value)
{
    OpenRGBEffectPage::EFCT->SetSpeed(value);
}

void OpenRGBEffectPage::DeviceListChangedCallback(void* ptr)
{
    OpenRGBEffectPage * this_obj = (OpenRGBEffectPage *)ptr;

    //OpenRGBEffectPage::DeviceListChanged();
    QMetaObject::invokeMethod(this_obj, "DeviceListChanged", Qt::QueuedConnection);
}

void OpenRGBEffectPage::DeviceListChanged()
{
    std::vector<RGBController*> RGBControllers = ORGBPlugin::RMPointer->GetRGBControllers();
    /*---------------------*\
    | Add Devices to list   |
    \*---------------------*/
    ui->SelectDevices->setRowCount(0);
    //qDebug() << RGBControllers.size();
    for (int i = 0; i < int(RGBControllers.size()); i++)
    {
        CreateDeviceSelection(RGBControllers[i]->name);
    }
}
