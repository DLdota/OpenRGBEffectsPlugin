#ifndef OPENRGBDIALOG2_H
#define OPENRGBDIALOG2_H

#include <vector>
#include "RGBController.h"
#include "NetworkClient.h"
#include "NetworkServer.h"

#include <QMainWindow>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QMenu>

namespace Ui
{
    class OpenRGBDialog2;
}

class Ui::OpenRGBDialog2 : public QMainWindow
{
    Q_OBJECT
public:
    explicit OpenRGBDialog2(QWidget *parent = 0);
    ~OpenRGBDialog2();

    static bool IsDarkTheme();
};

#endif // OPENRGBDIALOG2_H
