#ifndef WAVYUI_H
#define WAVYUI_H

#include "ui_WavyUi.h"
#include <QWidget>

namespace Ui {
class Wavy;
}

class WavyUi : public QWidget
{
    Q_OBJECT

public:
    /*-------------------------*\
    | Constructor / Destructor  |
    \*-------------------------*/
    explicit WavyUi(QWidget *parent = nullptr);
    ~WavyUi();

    void updateUi(int, int, int);

signals:
    void OnWaveFrequencyChanged(int) const;
    void OnWaveSpeedChanged(int) const;
    void OnOscillationSpeedChanged(int) const;

private slots:
    /*-------------*\
    | GUI Handling  |
    \*-------------*/


private:
    Ui::WavyUi *ui;
};

#endif // WAVYUI_H
