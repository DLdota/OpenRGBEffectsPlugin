#include "AudioVisualizerUi.h"

/*-------------------------*\
| Constructor / Destructor  |
\*-------------------------*/
AudioVisualizerUi::AudioVisualizerUi(QWidget *parent) : QWidget(parent), ui(new Ui::AudioVisualizerUi)
{
    ui->setupUi(this);

    vis_ptr = new Visualizer;
    vis_ptr->Initialize();

    ui->lineEdit_Normalization_Offset->setText(QString::number(vis_ptr->nrml_ofst));
    ui->lineEdit_Normalization_Scale->setText(QString::number(vis_ptr->nrml_scl));
    ui->lineEdit_Animation_Speed->setText(QString::number(vis_ptr->anim_speed));
    ui->lineEdit_Filter_Constant->setText(QString::number(vis_ptr->filter_constant));

    ui->lineEdit_Amplitude->setText(QString::number(vis_ptr->amplitude));
    ui->lineEdit_Background_Brightness->setText(QString::number(vis_ptr->bkgd_bright));
    ui->lineEdit_Average_Size->setText(QString::number(vis_ptr->avg_size));
    ui->lineEdit_Decay->setText(QString::number(vis_ptr->decay));
    ui->lineEdit_Background_Timeout->setText(QString::number(vis_ptr->background_timeout));

    ui->comboBox_FFT_Window_Mode->blockSignals(true);
    ui->comboBox_FFT_Window_Mode->addItem("None");
    ui->comboBox_FFT_Window_Mode->addItem("Hanning");
    ui->comboBox_FFT_Window_Mode->addItem("Hamming");
    ui->comboBox_FFT_Window_Mode->addItem("Blackman");
    ui->comboBox_FFT_Window_Mode->setCurrentIndex(vis_ptr->window_mode);
    ui->comboBox_FFT_Window_Mode->blockSignals(false);

    ui->comboBox_Background_Mode->blockSignals(true);
    for(int i = 0; i < VISUALIZER_NUM_PATTERNS; i++)
    {
        ui->comboBox_Background_Mode->addItem(visualizer_pattern_labels[i]);
    }
    ui->comboBox_Background_Mode->setCurrentIndex(vis_ptr->bkgd_mode);
    ui->comboBox_Background_Mode->blockSignals(false);

    ui->comboBox_Foreground_Mode->blockSignals(true);
    for(int i = 0; i < VISUALIZER_NUM_PATTERNS; i++)
    {
        ui->comboBox_Foreground_Mode->addItem(visualizer_pattern_labels[i]);
    }
    ui->comboBox_Foreground_Mode->setCurrentIndex(vis_ptr->frgd_mode);
    ui->comboBox_Foreground_Mode->blockSignals(false);

    ui->comboBox_Single_Color_Mode->blockSignals(true);
    for(int i = 0; i < VISUALIZER_NUM_SINGLE_COLOR; i++)
    {
        ui->comboBox_Single_Color_Mode->addItem(visualizer_single_color_labels[i]);
    }
    ui->comboBox_Single_Color_Mode->setCurrentIndex(vis_ptr->single_color_mode);
    ui->comboBox_Single_Color_Mode->blockSignals(false);

    ui->comboBox_Average_Mode->blockSignals(true);
    ui->comboBox_Average_Mode->addItem("Binning");
    ui->comboBox_Average_Mode->addItem("Low Pass");
    ui->comboBox_Average_Mode->setCurrentIndex(vis_ptr->avg_mode);
    ui->comboBox_Average_Mode->blockSignals(false);

    ui->comboBox_Audio_Device->blockSignals(true);
    for(unsigned int i = 0; i < AudioManager::get()->GetAudioDevices().size(); i++)
    {
        ui->comboBox_Audio_Device->addItem(AudioManager::get()->GetAudioDevices()[i]);
    }
    ui->comboBox_Audio_Device->setCurrentIndex(vis_ptr->audio_device_idx);
    ui->comboBox_Audio_Device->blockSignals(false);

    ui->checkBox_Reactive_Background->setChecked(vis_ptr->reactive_bkgd);
    ui->checkBox_Silent_Background->setChecked(vis_ptr->silent_bkgd);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(15);

    image = new QImage(256, 64, QImage::Format_RGB32);
    scene = new QGraphicsScene(this);
    ui->graphicsView_Visualization_Preview->setScene(scene);

    ui->ShowHideSettings->click();
}

AudioVisualizerUi::~AudioVisualizerUi()
{
    delete ui;
}


/*---------*\
| Settings  |
\*---------*/
void AudioVisualizerUi::LoadSettings(json Settings)
{
    vis_ptr->LoadSettings(Settings);

    ui->lineEdit_Normalization_Offset->setText(QString::number(vis_ptr->nrml_ofst));
    ui->lineEdit_Normalization_Scale->setText(QString::number(vis_ptr->nrml_scl));
    ui->lineEdit_Animation_Speed->setText(QString::number(vis_ptr->anim_speed));
    ui->lineEdit_Filter_Constant->setText(QString::number(vis_ptr->filter_constant));

    ui->lineEdit_Amplitude->setText(QString::number(vis_ptr->amplitude));
    ui->lineEdit_Background_Brightness->setText(QString::number(vis_ptr->bkgd_bright));
    ui->lineEdit_Average_Size->setText(QString::number(vis_ptr->avg_size));
    ui->lineEdit_Decay->setText(QString::number(vis_ptr->decay));
    ui->lineEdit_Background_Timeout->setText(QString::number(vis_ptr->background_timeout));

    ui->comboBox_Average_Mode->blockSignals(true);
    ui->comboBox_Average_Mode->setCurrentIndex(vis_ptr->avg_mode);
    ui->comboBox_Average_Mode->blockSignals(false);

    ui->comboBox_Background_Mode->blockSignals(true);
    ui->comboBox_Background_Mode->setCurrentIndex(vis_ptr->bkgd_mode);
    ui->comboBox_Background_Mode->blockSignals(false);

    ui->comboBox_FFT_Window_Mode->blockSignals(true);
    ui->comboBox_FFT_Window_Mode->setCurrentIndex(vis_ptr->window_mode);
    ui->comboBox_FFT_Window_Mode->blockSignals(false);

    ui->comboBox_Foreground_Mode->blockSignals(true);
    ui->comboBox_Foreground_Mode->setCurrentIndex(vis_ptr->frgd_mode);
    ui->comboBox_Foreground_Mode->blockSignals(false);

    ui->comboBox_Single_Color_Mode->blockSignals(true);
    ui->comboBox_Single_Color_Mode->setCurrentIndex(vis_ptr->single_color_mode);
    ui->comboBox_Single_Color_Mode->blockSignals(false);

    ui->checkBox_Reactive_Background->setChecked(vis_ptr->reactive_bkgd);
    ui->checkBox_Silent_Background->setChecked(vis_ptr->silent_bkgd);
}

json AudioVisualizerUi::SaveSettings()
{
    return vis_ptr->SaveSettings();
}

void AudioVisualizerUi::on_ShowHideSettings_clicked()
{
    if (Hiding)
    {
        Hiding = false;
        ui->MainFrame->show();
        ui->ToBottom->changeSize(0,0,QSizePolicy::Fixed,QSizePolicy::Fixed);
    }
    else
    {
        Hiding = true;
        ui->MainFrame->hide();
        ui->ToBottom->changeSize(0,0,QSizePolicy::Expanding,QSizePolicy::Expanding);
        this->adjustSize();
    }
}

/*---------*\
| Stepping  |
\*---------*/
void AudioVisualizerUi::Step(std::vector<OwnedControllerAndZones> Controllers, int FPS)
{
    vis_ptr->StepFrame(Controllers, FPS);
}


/*--------------*\
| State changes  |
\*--------------*/
void AudioVisualizerUi::EffectState(bool IsRunning)
{
    EffectActive = IsRunning;
    SetDevice();

    if (!IsRunning)
    {
        if (RegisteredForDevice)
        {
            AudioManager::get()->UnRegisterClient(vis_ptr->previous_audio_device_idx,this);
            RegisteredForDevice = false;
        }
    }
}

void AudioVisualizerUi::SetDevice()
{
    if (RegisteredForDevice)
    {
        AudioManager::get()->UnRegisterClient(vis_ptr->previous_audio_device_idx,this);
        RegisteredForDevice = false;
    }

    if (EffectActive)
    {
        AudioManager::get()->RegisterClient(vis_ptr->audio_device_idx,this);
        RegisteredForDevice = true;
    }
}


/*-------------*\
| GUI Handling  |
\*-------------*/
void AudioVisualizerUi::update()
{
    for(int x = 0; x < 256; x++)
    {
        for(int y = 0; y < 64; y++)
        {
            COLORREF input = vis_ptr->pixels_out->pixels[y][x];
            COLORREF bgr = RGB(GetBValue(input), GetGValue(input), GetRValue(input));
            image->setPixel(x, y, bgr);
        }
    }

    pixmap.convertFromImage(*image);
    scene->clear();
    scene->addPixmap(pixmap);

    if(vis_ptr->update_ui)
    {
        vis_ptr->update_ui = false;

        ui->lineEdit_Normalization_Offset->setText(QString::number(vis_ptr->nrml_ofst));
        ui->lineEdit_Normalization_Scale->setText(QString::number(vis_ptr->nrml_scl));
        ui->lineEdit_Animation_Speed->setText(QString::number(vis_ptr->anim_speed));
        ui->lineEdit_Filter_Constant->setText(QString::number(vis_ptr->filter_constant));

        ui->lineEdit_Amplitude->setText(QString::number(vis_ptr->amplitude));
        ui->lineEdit_Background_Brightness->setText(QString::number(vis_ptr->bkgd_bright));
        ui->lineEdit_Average_Size->setText(QString::number(vis_ptr->avg_size));
        ui->lineEdit_Decay->setText(QString::number(vis_ptr->decay));
        ui->lineEdit_Background_Timeout->setText(QString::number(vis_ptr->background_timeout));

        ui->comboBox_Average_Mode->blockSignals(true);
        ui->comboBox_Average_Mode->setCurrentIndex(vis_ptr->avg_mode);
        ui->comboBox_Average_Mode->blockSignals(false);

        ui->comboBox_Background_Mode->blockSignals(true);
        ui->comboBox_Background_Mode->setCurrentIndex(vis_ptr->bkgd_mode);
        ui->comboBox_Background_Mode->blockSignals(false);

        ui->comboBox_FFT_Window_Mode->blockSignals(true);
        ui->comboBox_FFT_Window_Mode->setCurrentIndex(vis_ptr->window_mode);
        ui->comboBox_FFT_Window_Mode->blockSignals(false);

        ui->comboBox_Foreground_Mode->blockSignals(true);
        ui->comboBox_Foreground_Mode->setCurrentIndex(vis_ptr->frgd_mode);
        ui->comboBox_Foreground_Mode->blockSignals(false);

        ui->comboBox_Single_Color_Mode->blockSignals(true);
        ui->comboBox_Single_Color_Mode->setCurrentIndex(vis_ptr->single_color_mode);
        ui->comboBox_Single_Color_Mode->blockSignals(false);

        ui->checkBox_Reactive_Background->setChecked(vis_ptr->reactive_bkgd);
        ui->checkBox_Silent_Background->setChecked(vis_ptr->silent_bkgd);
    }
}

void AudioVisualizerUi::on_lineEdit_Background_Brightness_textChanged(const QString &arg1)
{
vis_ptr->bkgd_bright = arg1.toInt();
vis_ptr->OnSettingsChanged();
}

void AudioVisualizerUi::on_lineEdit_Animation_Speed_textChanged(const QString &arg1)
{
    vis_ptr->anim_speed = arg1.toFloat();
    vis_ptr->OnSettingsChanged();
}


/*---------------------*\
| Amp, Size, and Decay  |
\*---------------------*/
void AudioVisualizerUi::on_lineEdit_Amplitude_textChanged(const QString &arg1)
{
    vis_ptr->amplitude = arg1.toInt();
    vis_ptr->OnSettingsChanged();
}

void AudioVisualizerUi::on_lineEdit_Average_Size_textChanged(const QString &arg1)
{
    vis_ptr->avg_size = arg1.toInt();
    vis_ptr->OnSettingsChanged();
}

void AudioVisualizerUi::on_lineEdit_Decay_textChanged(const QString &arg1)
{
    vis_ptr->decay = arg1.toInt();
    vis_ptr->OnSettingsChanged();
}


/*-------------*\
| Normalization |
\*-------------*/
void AudioVisualizerUi::on_lineEdit_Normalization_Offset_textChanged(const QString &arg1)
{
    vis_ptr->nrml_ofst = arg1.toFloat();
    vis_ptr->SetNormalization(vis_ptr->nrml_ofst, vis_ptr->nrml_scl);
    vis_ptr->OnSettingsChanged();
}

void AudioVisualizerUi::on_lineEdit_Normalization_Scale_textChanged(const QString &arg1)
{
    vis_ptr->nrml_scl = arg1.toFloat();
    vis_ptr->SetNormalization(vis_ptr->nrml_ofst, vis_ptr->nrml_scl);
    vis_ptr->OnSettingsChanged();
}


/*------------------------------*\
| Color and Brightness Settings  |
\*------------------------------*/
void AudioVisualizerUi::on_comboBox_FFT_Window_Mode_currentIndexChanged(int index)
{
    vis_ptr->window_mode = index;
    vis_ptr->OnSettingsChanged();
}

void AudioVisualizerUi::on_comboBox_Background_Mode_currentIndexChanged(int index)
{
    vis_ptr->bkgd_mode = index;
    vis_ptr->OnSettingsChanged();
}

void AudioVisualizerUi::on_comboBox_Foreground_Mode_currentIndexChanged(int index)
{
    vis_ptr->frgd_mode = index;
    vis_ptr->OnSettingsChanged();
}

void AudioVisualizerUi::on_comboBox_Single_Color_Mode_currentIndexChanged(int index)
{
    vis_ptr->single_color_mode = index;
    vis_ptr->OnSettingsChanged();
}

void AudioVisualizerUi::on_comboBox_Average_Mode_currentIndexChanged(int index)
{
    vis_ptr->avg_mode = index;
    vis_ptr->OnSettingsChanged();
}

void AudioVisualizerUi::on_checkBox_Reactive_Background_clicked(bool checked)
{
    vis_ptr->reactive_bkgd = checked;

    if (vis_ptr->reactive_bkgd == true)
    {
        vis_ptr->silent_bkgd = false;
        ui->checkBox_Silent_Background->setChecked(false);
    }
    vis_ptr->OnSettingsChanged();
}

void AudioVisualizerUi::on_comboBox_Audio_Device_currentIndexChanged(int index)
{
    vis_ptr->previous_audio_device_idx = vis_ptr->audio_device_idx;
    vis_ptr->audio_device_idx = index;
    SetDevice();
}

void AudioVisualizerUi::on_lineEdit_Filter_Constant_textChanged(const QString &arg1)
{
    vis_ptr->filter_constant = arg1.toFloat();
    if(vis_ptr->filter_constant > 1.0f)
    {
        vis_ptr->filter_constant = 1.0f;
    }
    else if(vis_ptr->filter_constant < 0.0f)
    {
        vis_ptr->filter_constant = 0.0f;
    }
    vis_ptr->OnSettingsChanged();
}

void AudioVisualizerUi::on_checkBox_Silent_Background_clicked(bool checked)
{
    vis_ptr->silent_bkgd = checked;

    if (vis_ptr->silent_bkgd == true)
    {
        vis_ptr->reactive_bkgd = false;
        ui->checkBox_Reactive_Background->setChecked(false);
    }

    vis_ptr->OnSettingsChanged();
}

void AudioVisualizerUi::on_lineEdit_Background_Timeout_textChanged(const QString &arg1)
{
    vis_ptr->background_timeout = arg1.toInt();

    if (vis_ptr->update_ui == false)
    {
        vis_ptr->background_timer = 0;
    }

    vis_ptr->OnSettingsChanged();
}
