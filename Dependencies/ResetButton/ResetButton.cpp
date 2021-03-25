#include "ResetButton.h"

ResetButton::ResetButton(int DIndex, int ZIndex, QWidget* Parent)
{
    this->setParent(Parent);

    std::string IconStr = ":Reset";
    if (ORGBPlugin::DarkTheme) IconStr.append("_dark");
    IconStr.append(".png");
    this->setIcon(QIcon(IconStr.c_str()));

    DeviceIndex = DIndex;
    ZoneIndex = ZIndex;

    connect(this,SIGNAL(clicked()),this,SLOT(EmitUnlock()));
}

void ResetButton::EmitUnlock()
{
    emit Unlock(DeviceIndex, ZoneIndex);
}
