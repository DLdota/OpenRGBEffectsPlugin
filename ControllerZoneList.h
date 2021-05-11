#ifndef CONTROLLERZONELIST_H
#define CONTROLLERZONELIST_H

#include <QWidget>

namespace Ui {
class ControllerZoneList;
}

class ControllerZoneList : public QWidget
{
    Q_OBJECT

public:
    explicit ControllerZoneList(QWidget *parent = nullptr);
    ~ControllerZoneList();

    void DisableControls();
    void EnableControls();

private:
    Ui::ControllerZoneList *ui;
};

#endif // CONTROLLERZONELIST_H
