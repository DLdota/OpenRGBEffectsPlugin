#include "WavyUi.h"

WavyUi::WavyUi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WavyUi)
{
    ui->setupUi(this);

    ui->oscillation_speed_slider->setMinimum(1);
    ui->oscillation_speed_slider->setMaximum(200);

    ui->wave_freq_slider->setMinimum(1);
    ui->wave_freq_slider->setMaximum(20);

    ui->wave_speed_slider->setMinimum(1);
    ui->wave_speed_slider->setMaximum(200);

    connect(ui->wave_freq_slider, SIGNAL(valueChanged(int)), SIGNAL(OnWaveFrequencyChanged(int)));
    connect(ui->wave_speed_slider, SIGNAL(valueChanged(int)), SIGNAL(OnWaveSpeedChanged(int)));
    connect(ui->oscillation_speed_slider, SIGNAL(valueChanged(int)), SIGNAL(OnOscillationSpeedChanged(int)));
}

WavyUi::~WavyUi()
{
    delete ui;
}

void WavyUi::updateUi(int WaveFrequency, int WaveSpeed, int OscillationSpeed)
{
    ui->oscillation_speed_slider->setValue(OscillationSpeed);
    ui->wave_freq_slider->setValue(WaveFrequency);
    ui->wave_speed_slider->setValue(WaveSpeed);
}
