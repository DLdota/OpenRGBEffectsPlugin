#include "AudioSettings.h"
#include "ui_AudioSettings.h"
#include "Audio/AudioManager.h"
#include <QSlider>

AudioSettings::AudioSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AudioSettings)
{
    ui->setupUi(this);

    /*------------------------------------*\
    | Fill in Audio devices                |
    \*------------------------------------*/
    // todo: default device

    for(const char * str : AudioManager::get()->GetAudioDevices())
    {
        ui->audio_device->addItem(QString::fromUtf8(str));
    }

    /*------------------------------------*\
    | Fill in Modes                        |
    \*------------------------------------*/
    ui->window_mode->blockSignals(true);
    ui->window_mode->addItem("None");
    ui->window_mode->addItem("Hanning");
    ui->window_mode->addItem("Hamming");
    ui->window_mode->addItem("Blackman");
    ui->window_mode->blockSignals(false);

    ui->avg_mode->blockSignals(true);
    ui->avg_mode->addItem("Binning");
    ui->avg_mode->addItem("Low Pass");
    ui->avg_mode->blockSignals(false);

    /*------------------------------------*\
    | Prepare dialog                       |
    \*------------------------------------*/
    setWindowTitle("Audio settings");
    setModal(false);

    /*------------------------------------*\
    | Bind equalizer                       |
    \*------------------------------------*/
    eqs = {
        ui->eq_01, ui->eq_02, ui->eq_03, ui->eq_04, ui->eq_05, ui->eq_06, ui->eq_07, ui->eq_08,
        ui->eq_09, ui->eq_10, ui->eq_11, ui->eq_12, ui->eq_13, ui->eq_14, ui->eq_15, ui->eq_16
    };

    for(unsigned int eq = 0; eq < eqs.size(); eq++)
    {
        connect(eqs[eq], &QSlider::valueChanged, [=](int value){
            settings->equalizer[eq] = value * 0.01;
        });
    }

}

AudioSettings::~AudioSettings()
{
    delete ui;
}

void AudioSettings::SetSettings(Audio::AudioSettingsStruct* settings)
{
    this->settings = settings;

    SetSettingsValues(*settings);
    SetEQValues(*settings);
}

void AudioSettings::SetSettingsValues(const Audio::AudioSettingsStruct& s)
{
    ui->audio_device->setCurrentIndex(s.audio_device);
    ui->amplitude->setValue(s.amplitude);
    ui->avg_mode->setCurrentIndex(s.avg_mode);
    ui->avg_size->setValue(s.avg_size);
    ui->window_mode->setCurrentIndex(s.window_mode);
    ui->decay->setValue(s.decay);
    ui->filter_constant->setValue(s.filter_constant);
    ui->nrml_ofst->setValue(s.nrml_ofst);
    ui->nrml_scl->setValue(s.nrml_scl);
}

void AudioSettings::SetEQValues(const Audio::AudioSettingsStruct& s)
{
    for(unsigned int eq = 0; eq < eqs.size(); eq++)
    {
        eqs[eq]->setValue(s.equalizer[eq] * 100);
    }
}

void AudioSettings::on_audio_device_currentIndexChanged(int idx)
{
    printf("AudioSettings::on_devices_currentIndexChanged: %d\n", idx);
    emit AudioDeviceChanged(idx);
}

void AudioSettings::on_amplitude_valueChanged(int value)
{
    settings->amplitude = value;
}

void AudioSettings::on_avg_mode_currentIndexChanged(int value)
{
    settings->avg_mode = value;
}

void AudioSettings::on_avg_size_valueChanged(int value)
{
    settings->avg_size = value;
}

void AudioSettings::on_window_mode_currentIndexChanged(int value)
{
    settings->window_mode = value;
}

void AudioSettings::on_decay_valueChanged(int value)
{
    settings->decay = value;
}

void AudioSettings::on_filter_constant_valueChanged(double value)
{
    settings->filter_constant = value;
}

void AudioSettings::on_nrml_ofst_valueChanged(double value)
{
    settings->nrml_ofst = value;
    emit NormalizationChanged();
}

void AudioSettings::on_nrml_scl_valueChanged(double value)
{
    settings->nrml_scl = value;
    emit NormalizationChanged();
}

void AudioSettings::on_restoreDefaultSettings_clicked()
{
    Audio::AudioSettingsStruct default_values;
    SetSettingsValues(default_values);
}

void AudioSettings::on_restoreDefaultEQ_clicked()
{
    Audio::AudioSettingsStruct default_values;
    SetEQValues(default_values);
}
