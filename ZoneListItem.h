#ifndef ZONELISTITEM_H
#define ZONELISTITEM_H

#include <QWidget>

namespace Ui {
class ZoneListItem;
}

class ZoneListItem : public QWidget
{
    Q_OBJECT

public:
    ZoneListItem(QString);
    ~ZoneListItem();

    void SetEnableChecked(bool);
    void SetReverseChecked(bool);

    void DisableControls();
    void EnableControls();
    void ToggleBrightnessSlider();

    bool IsEnabled();
    bool IsReversed();

    void SetBrightness(int);

private slots:
    void on_enable_toggled(bool);
    void on_reverse_toggled(bool);
    void on_brightness_valueChanged(int);

signals:
    void Enabled(bool);
    void Reversed(bool);
    void BrightnessChanged(int);

private:
    Ui::ZoneListItem *ui;
    void UpdateCheckState();
};

#endif // ZONELISTITEM_H
