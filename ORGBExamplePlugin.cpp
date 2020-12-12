#include "ORGBExamplePlugin.h"
#include "Dependencies/ResourceManager.h"

std::string ORGBPlugin::PluginName() const
{
    return "ExamplePlugin";
}

std::string ORGBPlugin::PluginDesc() const
{
    return "This is an Example plugin for OpenRGB";
}

std::string ORGBPlugin::PluginLocal() const
{
    return "InfoTab";
}

QWidget* ORGBPlugin::CreateGUI(QWidget *Parent, ResourceManager *RM) const
{
    QWidget *ORGBExamplePage = new QWidget(Parent);
    QLabel *ORGBExampleLabel = new QLabel(ORGBExamplePage);

    QPushButton *ORGBExamplePushButton = new QPushButton(ORGBExamplePage);
    qDebug() << ORGBExamplePushButton->objectName();
    connect(ORGBExamplePushButton,SIGNAL(clicked()) ,this , SLOT(on_ExampleButton_clicked()));

    ORGBExampleLabel->setText("This is an example page added by plugins");
    return ORGBExamplePage;
}

void ORGBPlugin::on_ExampleButton_clicked()
{
    QDialog *ButtonDialog = new QDialog();
    ButtonDialog->setModal(true);
    QLabel *DialogText = new QLabel(ButtonDialog);
    DialogText->setText("This is the result of the button being clicked");
    ButtonDialog->show();
}
