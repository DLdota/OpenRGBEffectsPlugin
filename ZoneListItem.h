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

    bool IsEnabled();
    bool IsReversed();

private slots:
    void on_enable_clicked();
    void on_reverse_clicked();

signals:
    void Enabled(bool);
    void Reversed(bool);

private:
    Ui::ZoneListItem *ui;
};

#endif // ZONELISTITEM_H
