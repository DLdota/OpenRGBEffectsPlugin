#ifndef AUDIOVISUALIZERUI_H
#define AUDIOVISUALIZERUI_H

#include "ui_AudioVisualizerUi.h"
#include <QWidget>
#include <QTimer>
#include <QGraphicsScene>
#include "Visualizer.h"
#include "RGBEffect.h"
#include "AudioManager.h"

namespace Ui {
class AudioVisualizer;
}

class AudioVisualizerUi : public QWidget
{
    Q_OBJECT

public:
    /*-------------------------*\
    | Constructor / Destructor  |
    \*-------------------------*/
    explicit AudioVisualizerUi(QWidget *parent = nullptr);
    ~AudioVisualizerUi();

    /*---------*\
    | Settings  |
    \*---------*/
    void LoadSettings(json Settings);
    json SaveSettings();

    /*---------*\
    | Stepping  |
    \*---------*/
    void Step(std::vector<OwnedControllerAndZones> Controllers, int FPS);

    /*--------------*\
    | State changes  |
    \*--------------*/
    void EffectState(bool IsRunning);

private slots:
    /*-------------*\
    | GUI Handling  |
    \*-------------*/
    void update();
    void on_lineEdit_Background_Brightness_textChanged(const QString &arg1);
    void on_lineEdit_Animation_Speed_textChanged(const QString &arg1);

    /*---------------------*\
    | Amp, Size, and Decay  |
    \*---------------------*/
    void on_lineEdit_Amplitude_textChanged(const QString &arg1);
    void on_lineEdit_Average_Size_textChanged(const QString &arg1);
    void on_lineEdit_Decay_textChanged(const QString &arg1);

    /*-------------*\
    | Normalization |
    \*-------------*/
    void on_lineEdit_Normalization_Offset_textChanged(const QString &arg1);
    void on_lineEdit_Normalization_Scale_textChanged(const QString &arg1);

    /*------------------------------*\
    | Color and Brightness Settings  |
    \*------------------------------*/
    void on_comboBox_FFT_Window_Mode_currentIndexChanged(int index);
    void on_comboBox_Background_Mode_currentIndexChanged(int index);
    void on_comboBox_Foreground_Mode_currentIndexChanged(int index);
    void on_comboBox_Single_Color_Mode_currentIndexChanged(int index);
    void on_comboBox_Average_Mode_currentIndexChanged(int index);
    void on_checkBox_Reactive_Background_clicked(bool checked);
    void on_comboBox_Audio_Device_currentIndexChanged(int index);
    void on_lineEdit_Filter_Constant_textChanged(const QString &arg1);
    void on_checkBox_Silent_Background_clicked(bool checked);
    void on_lineEdit_Background_Timeout_textChanged(const QString &arg1);

    void on_ShowHideSettings_clicked();

private:
    /*---------*\
    | GUI bits  |
    \*---------*/
    QGraphicsScene* scene;
    QPixmap pixmap;
    QImage* image;
    QTimer* timer;

    /*--------------*\
    | State changes  |
    \*--------------*/
    bool Hiding = false;
    bool RegisteredForDevice = false;
    bool EffectActive = false;
    void SetDevice();

    /*-----------------------*\
    | Pointers                |
    \*-----------------------*/
    Visualizer* vis_ptr;
    Ui::AudioVisualizerUi *ui;
};

#endif // AUDIOVISUALIZERUI_H
