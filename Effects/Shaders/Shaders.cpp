#include "Shaders.h"
#include "AudioManager.h"

/*
TODO:

- add more inputs
- custom attributes to pass

- bind music
- gl_FragColor deprecation
- segfaults after a while - mutex added, lets see.
*/

REGISTER_EFFECT(Shaders);

Shaders::Shaders(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::Shaders)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "Shaders";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Shaders effect";

    EffectDetails.IsReversable = false;
    EffectDetails.MaxSpeed     = 2000;
    EffectDetails.MinSpeed     = 0;
    EffectDetails.UserColors   = 0;
    EffectDetails.AllowOnlyFirst = false;

    EffectDetails.MaxSlider2Val = 0;
    EffectDetails.MinSlider2Val = 0;
    EffectDetails.Slider2Name   = "";

    EffectDetails.HasCustomWidgets = true;
    EffectDetails.HasCustomSettings = true;

    shader_renderer = new ShaderRenderer(this);
    editor = new GLSLCodeEditor(this);

    ui->preview->hide();

    connect(editor, &GLSLCodeEditor::Applied, [this](){
        Apply();
    });

    connect(shader_renderer, &ShaderRenderer::Image, [this](const QImage& image){
        image_mutex.lock();
        this->image = image;
        QMetaObject::invokeMethod(ui->preview, "setPixmap",
                                  Qt::QueuedConnection, Q_ARG(QPixmap, QPixmap::fromImage(image)));
        //ui->preview->setPixmap(QPixmap::fromImage(image));
        image_mutex.unlock();
    });

    connect(shader_renderer, &ShaderRenderer::Log, editor, &GLSLCodeEditor::SetLog);

    /*-----------------------------------------------*\
    | List the embeded shadders, add them to the      |
    | combo box                                       |
    \*-----------------------------------------------*/
    QDirIterator it(":/shaders");
    QStringList shader_list;

    while (it.hasNext())
    {        
        shader_list << it.next();
    }

    shader_list.sort();

    for(const QString& path: shader_list)
    {
        shader_paths.push_back(path);
        ui->shaders->addItem(path.split( "/" ).last());
    }

    /*-----------------------------------------------*\
    | List audio devices                              |
    \*-----------------------------------------------*/
    std::vector<char *> devices = AudioManager::get()->GetAudioDevices();

    for(const char * str : devices)
    {
        ui->audio_device->addItem(QString::fromLocal8Bit(str));
    }

    ui->audio_frame->hide();

    hanning(win_hanning, 256);

    float offset            = 0.04f;
    float scale             = 0.5f;

    /*------------------------------------*\
    | Fill in Normalization and FFT array  |
    \*------------------------------------*/
    for (int i = 0; i < 256; i++)
    {
        fft[i] = 0.0f;
        fft_nrml[i] = offset + (scale * (i / 256.0f));

    }

     shader_data.iTime = 0;
     shader_data.iAudio = fft_fltr;
}

void Shaders::SetFPS(unsigned int value)
{
    FPS = value;
    shader_renderer->SetFPS(value);
}

void Shaders::EffectState(bool state)
{
    EffectEnabled = state;

    if(state)
    {
        shader_renderer->Start();

        if(use_audio)
        {
            StartAudio();
        }
    }
    else
    {
        shader_renderer->Stop();

        if(use_audio)
        {
            StopAudio();
        }
    }    
}

Shaders::~Shaders()
{
    delete ui;
}

void Shaders::DefineExtraOptions(QLayout* layout)
{
    layout->addWidget(this);
}

void Shaders::LoadShader(std::string fragmentShader)
{    
    editor->SetContent(QString::fromStdString(fragmentShader));
    shader_data.iTime = 0;
    shader_renderer->Update(shader_data);
    Apply();
}


void Shaders::Apply()
{    
    std::string fragmentShader = editor->GetContent();
    std::string version = editor->GetVersion();
    shader_renderer->SetShader(version, fragmentShader);
}

void Shaders::Resize()
{
    shader_renderer->Resize(width, height);
}

void Shaders::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    shader_data.iTime += 0.001 * Speed / (float) FPS;

    if(use_audio)
    {
        HandleAudioCapture();
    }

    image_mutex.lock();

    shader_renderer->Update(shader_data);

    if(image.isNull())
    {
        image_mutex.unlock();
        return;
    }

    QImage copy = image.copy();

    image_mutex.unlock();

    for(ControllerZone* controller_zone : controller_zones)
    {
        unsigned int start_idx = controller_zone->start_idx();
        zone_type ZT = controller_zone->type();

        if(ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            unsigned int width = controller_zone->leds_count();
            unsigned int height = 1;

            QImage scaled = copy.scaled(width, height);

            for(unsigned int i = 0; i < width; i++)
            {
                QColor color = scaled.pixelColor(i, 0);
                controller_zone->controller->SetLED(start_idx + i, ColorUtils::fromQColor(color));
            }

        }
        else if(ZT == ZONE_TYPE_MATRIX)
        {
            unsigned int width = controller_zone->matrix_map_width();
            unsigned int height = controller_zone->matrix_map_height();
            unsigned int * map = controller_zone->map();

            QImage scaled = copy.scaled(width, height);

            for(unsigned int h = 0; h < height; h++)
            {
                for(unsigned int w = 0; w <  width; w++)
                {
                    QColor color = scaled.pixelColor(w, h);

                    unsigned int led_num = map[h * width + w];
                    controller_zone->controller->SetLED(start_idx + led_num, ColorUtils::fromQColor(color));
                }
            }

        }
    }
}


void Shaders::LoadCustomSettings(json Settings)
{
    if(Settings.contains("shader_name"))
        ui->shaders->setCurrentText(QString::fromStdString(Settings["shader_name"]));

    if(Settings.contains("fragmentShader"))
        editor->SetContent(QString::fromStdString(Settings["fragmentShader"]));

    if(Settings.contains("shaderVersion"))
        editor->SetVersion(QString::fromStdString(Settings["shaderVersion"]));

    if(Settings.contains("width"))
        ui->width->setValue(Settings["width"]);

    if(Settings.contains("height"))
        ui->height->setValue(Settings["height"]);

    if(Settings.contains("show_rendering"))
        ui->show_rendering->setChecked(Settings["show_rendering"]);

    if(Settings.contains("use_audio"))
        ui->use_audio->setChecked(Settings["use_audio"]);

    if(Settings.contains("audio_device_idx"))
        ui->audio_device->setCurrentIndex(Settings["audio_device_idx"]);

    if(Settings.contains("amplitude"))
        ui->amplitude->setValue(Settings["amplitude"]);

    if(Settings.contains("decay"))
        ui->decay->setValue(Settings["decay"]);

    if(Settings.contains("avg_size"))
        ui->average->setValue(Settings["avg_size"]);

    Apply();
}

json Shaders::SaveCustomSettings(json Settings)
{
    Settings["shader_name"]      = ui->shaders->currentText().toStdString();
    Settings["fragmentShader"]   = editor->GetContent();
    Settings["shaderVersion"]    = editor->GetVersion();
    Settings["width"]            = width;
    Settings["height"]           = height;
    Settings["show_rendering"]   = show_rendering;

    Settings["use_audio"]        = use_audio;
    Settings["audio_device_idx"] = audio_device_idx;
    Settings["amplitude"]        = amplitude;
    Settings["decay"]            = decay;
    Settings["avg_size"]         = avg_size;

    return Settings;
}

void Shaders::StartAudio()
{
    AudioManager::get()->RegisterClient(audio_device_idx, this);
}

void Shaders::StopAudio()
{
    AudioManager::get()->UnRegisterClient(audio_device_idx, this);
}

void Shaders::HandleAudioCapture()
{
    float fft_tmp[512];

    for (int i = 0; i < 256; i++)
    {
        /*------------------*\
        | Clear the buffers  |
        \*------------------*/
        fft_tmp[i] = 0;

        /*----------------------*\
        | Decay previous values  |
        \*----------------------*/
        fft[i] = fft[i] * ((float(decay) / 100.0f / (60 / FPS)));
    }

    AudioManager::get()->Capture(audio_device_idx, fft_tmp);

#ifdef _WIN32
    for (int i = 0; i < 512; i++)
    {
        fft_tmp[i] *= amplitude;
    }
#else
    for (int i = 0; i < 512; i++)
    {
        fft_tmp[i] = (fft_tmp[i] - 128.0f) * (amplitude / 128.0f);
    }
#endif

    apply_window(fft_tmp, win_hanning, 256);

    /*------------------------*\
    | Run the FFT calculation  |    void EffectState(bool) override;

    \*------------------------*/
    rfft(fft_tmp, 256, 1);

    fft_tmp[0] = fft_tmp[2];

    apply_window(fft_tmp, fft_nrml, 256);

    /*----------------------*\
    | Compute FFT magnitude  |
    \*----------------------*/
    for (int i = 0; i < 128; i += 2)
    {
        float fftmag;

        /*---------------------------------------------------------------------------------*\
        | Compute magnitude from real and imaginary components of FFT and apply simple LPF  |
        \*---------------------------------------------------------------------------------*/
        fftmag = (float)sqrt((fft_tmp[i] * fft_tmp[i]) + (fft_tmp[i + 1] * fft_tmp[i + 1]));

        /*----------------------------------------------------------------------------------------*\
        | Apply a slight logarithmic filter to minimize noise from very low amplitude frequencies  |
        \*----------------------------------------------------------------------------------------*/
        fftmag = ( 0.5f * log10(1.1f * fftmag) ) + ( 0.9f * fftmag );

        /*---------------------------*\
        | Limit FFT magnitude to 1.0  |
        \*---------------------------*/
        if (fftmag > 1.0f)
        {
            fftmag = 1.0f;
        }

        /*----------------------------------------------------------*\
        | Update to new values only if greater than previous values  |
        \*----------------------------------------------------------*/
        if (fftmag > fft[i*2])
        {
            fft[i*2] = fftmag;;
        }

        /*----------------------------*\
        | Prevent from going negative  |
        \*----------------------------*/
        if (fft[i*2] < 0.0f)
        {
            fft[i*2] = 0.0f;
        }

        /*--------------------------------------------------------------------*\
        | Set odd indexes to match their corresponding even index, as the FFT  |
        | input array uses two indices for one value (real+imaginary)          |
        \*--------------------------------------------------------------------*/
        fft[(i * 2) + 1] = fft[i * 2];
        fft[(i * 2) + 2] = fft[i * 2];
        fft[(i * 2) + 3] = fft[i * 2];
    }


    /*--------------------------------------------*\
    | Apply averaging over given number of values  |
    \*--------------------------------------------*/
    unsigned int k;
    float sum1 = 0;
    float sum2 = 0;

    for (k = 0; k < avg_size; k++)
    {
        sum1 += fft[k];
        sum2 += fft[255 - k];
    }
    /*------------------------------*\
    | Compute averages for end bars  |
    \*------------------------------*/
    sum1 = sum1 / k;
    sum2 = sum2 / k;

    for (k = 0; k < avg_size; k++)
    {
        fft[k] = sum1;
        fft[255 - k] = sum2;
    }

    for (unsigned int i = 0; i < (256 - avg_size); i += avg_size)
    {
        float sum = 0;
        for (unsigned int j = 0; j < avg_size; j += 1)
        {
            sum += fft[i + j];
        }

        float avg = sum / avg_size;

        for (unsigned int j = 0; j < avg_size; j += 1)
        {
            fft[i + j] = avg;
        }
    }

    for(int i = 0; i < 256; i++)
    {
        fft_fltr[i] = fft_fltr[i] + (filter_constant * (fft[i] - fft_fltr[i]));
    }
}

void Shaders::on_show_rendering_stateChanged(int state)
{
    ui->preview->setVisible(state);
    show_rendering = state;
}

void Shaders::on_use_audio_stateChanged(int state)
{
    ui->audio_frame->setVisible(state);

    use_audio = state;

    if(use_audio && EffectEnabled)
    {
        StartAudio();
    }

    if(!use_audio && EffectEnabled)
    {
        StopAudio();
    }
}

void Shaders::on_audio_device_currentIndexChanged(int idx)
{
    if(!use_audio)
    {
        audio_device_idx = idx;
        return;
    }

    bool was_running = EffectEnabled;

    if(EffectEnabled)
    {
        StopAudio();
    }

    audio_device_idx = idx;

    if(was_running)
    {
        StartAudio();
    }
}

void Shaders::on_shaders_currentIndexChanged(int idx)
{
    current_shader_idx = idx;

    QFile frag(shader_paths[current_shader_idx]);

    frag.open(QFile::ReadOnly | QFile::Text);

    QTextStream frag_in(&frag);

    LoadShader(frag_in.readAll().toStdString());
}

void Shaders::on_width_valueChanged(int value)
{
    width = value;
    Resize();
}

void Shaders::on_height_valueChanged(int value)
{
    height = value;
    Resize();
}

void Shaders::on_edit_clicked()
{
    editor->show();
}

void Shaders::on_amplitude_valueChanged(int value)
{
    amplitude = value;
}

void Shaders::on_decay_valueChanged(int value)
{
    decay = value;
}

void Shaders::on_average_valueChanged(int value)
{
    avg_size = value;
}

