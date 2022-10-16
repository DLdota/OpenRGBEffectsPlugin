#ifndef EFFECTTABHEADER_H
#define EFFECTTABHEADER_H

#include <QWidget>
#include "RGBEffect.h"

namespace Ui {
class EffectTabHeader;
}

class EffectTabHeader : public QWidget
{
    Q_OBJECT

public:
    explicit EffectTabHeader(QWidget *parent = nullptr, RGBEffect* effect = nullptr);
    ~EffectTabHeader();

    void ToogleRunningIndicator(bool);

signals:
    void CloseRequest();

private slots:
    void on_close_clicked();
    void on_rename_clicked();

private:
    Ui::EffectTabHeader *ui;
};

#endif // EFFECTTABHEADER_H
