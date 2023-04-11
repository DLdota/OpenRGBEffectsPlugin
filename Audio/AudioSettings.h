#ifndef AUDIOSETTINGS_H
#define AUDIOSETTINGS_H

#include <QWidget>
#include <QDialog>
#include <QSlider>
#include "AudioSettingsStruct.h"

namespace Ui
{
    class AudioSettings;
}

class AudioSettings : public QDialog
{
    Q_OBJECT

public:
    explicit AudioSettings(QWidget *parent = nullptr);
    ~AudioSettings();

    void SetSettings(Audio::AudioSettingsStruct*);

private slots:
    void on_audio_device_currentIndexChanged(int);

    void on_amplitude_valueChanged(int);
    void on_avg_mode_currentIndexChanged(int);
    void on_avg_size_valueChanged(int);
    void on_window_mode_currentIndexChanged(int);
    void on_decay_valueChanged(int);
    void on_filter_constant_valueChanged(double);
    void on_nrml_ofst_valueChanged(double);
    void on_nrml_scl_valueChanged(double);
    void on_restoreDefaultSettings_clicked();
    void on_restoreDefaultEQ_clicked();

signals:
    void AudioDeviceChanged(int);
    void NormalizationChanged();

private:
    Ui::AudioSettings *ui;
    Audio::AudioSettingsStruct* settings;
    std::vector<QSlider *> eqs;

    void SetSettingsValues(const Audio::AudioSettingsStruct&);
    void SetEQValues(const Audio::AudioSettingsStruct&);
};

#endif // AUDIOSETTINGS_H
