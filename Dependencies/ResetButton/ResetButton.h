#ifndef RESETBUTTON_H
#define RESETBUTTON_H

#include <QPushButton>
#include <ORGBEffectPlugin.h>

class ResetButton : public QPushButton
{
    Q_OBJECT
public:
    ResetButton(int DeviceIndex, int ZoneIndex, QWidget* Parent = nullptr);

signals:
    void Unlock(int,int);

private:
    int DeviceIndex;
    int ZoneIndex;

private slots:
    void EmitUnlock();
};

#endif // RESETBUTTON_H
