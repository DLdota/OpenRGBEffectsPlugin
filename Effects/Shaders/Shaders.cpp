#include "Shaders.h"
#include "Audio/AudioManager.h"
#include "OpenRGBEffectSettings.h"

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
    EffectDetails.EffectDescription = "Unleash the power of OpenRGB with GL shaders";
    EffectDetails.MaxSpeed     = 2000;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.HasCustomSettings = true;
    EffectDetails.SupportsRandom = false;

    shader_renderer = new ShaderRenderer(this);

    editor = new GLSLCodeEditor(this, shader_renderer->Program());

    ui->preview->hide();

    // Connect slots
    connect(editor, &GLSLCodeEditor::Applied, [this](ShaderProgram* program){
        program->Resize(width, height);
        shader_renderer->SetProgram(program);
    });

    connect(shader_renderer, &ShaderRenderer::Image, [this](const QImage& image){        
        image_mutex.lock();
        this->image = image;
        QMetaObject::invokeMethod(ui->preview, "setPixmap", Qt::QueuedConnection, Q_ARG(QPixmap, QPixmap::fromImage(image)));
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

    /*--------------------------*\
    | Setup audio                |
    \*--------------------------*/

    memcpy(&audio_settings_struct, &OpenRGBEffectSettings::globalSettings.audio_settings,sizeof(Audio::AudioSettingsStruct));

    audio_signal_processor.SetNormalization(&audio_settings_struct);

    connect(&audio_settings, &AudioSettings::AudioDeviceChanged, this, &Shaders::OnAudioDeviceChanged);
    connect(&audio_settings, &AudioSettings::NormalizationChanged,[=]{
        audio_signal_processor.SetNormalization(&audio_settings_struct);
    });

    audio_settings.SetSettings(&audio_settings_struct);

    SetSpeed(1000);
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

void Shaders::Resize()
{
    shader_renderer->Program()->Resize(width, height);
}

void Shaders::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    time += 0.001 * Speed / (float) FPS;

    if(use_audio)
    {
        audio_signal_processor.Process(FPS, &audio_settings_struct);
    }

    if(!shader_renderer->isRunning())
    {
        return;
    }

    shader_renderer->uniforms.iTime = invert_time ? - time : time;
    shader_renderer->uniforms.iAudio = (float*) audio_signal_processor.Data().fft_fltr;

    image_mutex.lock();

    if(image.isNull())
    {
        image_mutex.unlock();
        return;
    }

    QImage copy = image.copy();

    image_mutex.unlock();

    for(ControllerZone* controller_zone : controller_zones)
    {
        zone_type ZT = controller_zone->type();

        if(ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            unsigned int width = controller_zone->leds_count();
            unsigned int height = 1;

            QImage scaled = copy.scaled(width, height);

            for(unsigned int i = 0; i < width; i++)
            {
                QColor color = scaled.pixelColor(i, 0);
                controller_zone->SetLED(i, ColorUtils::fromQColor(color), Brightness);
            }

        }
        else if(ZT == ZONE_TYPE_MATRIX)
        {
            unsigned int width = controller_zone->matrix_map_width();
            unsigned int height = controller_zone->matrix_map_height();

            QImage scaled = copy.scaled(width, height);

            for(unsigned int h = 0; h < height; h++)
            {
                for(unsigned int w = 0; w <  width; w++)
                {
                    QColor color = scaled.pixelColor(w, h);

                    unsigned int led_num = controller_zone->map()[h * width + w];
                    controller_zone->SetLED(led_num, ColorUtils::fromQColor(color), Brightness);
                }
            }

        }
    }
}

void Shaders::LoadCustomSettings(json Settings)
{
    if(Settings.contains("shader_name"))
        ui->shaders->setCurrentText(QString::fromStdString(Settings["shader_name"]));

    if(Settings.contains("shader_program"))
    {
        shader_renderer->SetProgram(ShaderProgram::FromJSON(Settings["shader_program"]));
        editor->SetProgram(shader_renderer->Program());
    }

    if(Settings.contains("width"))
        ui->width->setValue(Settings["width"]);

    if(Settings.contains("height"))
        ui->height->setValue(Settings["height"]);

    if(Settings.contains("show_rendering"))
        ui->show_rendering->setChecked(Settings["show_rendering"]);

    if(Settings.contains("invert_time"))
        ui->invert_time->setChecked(Settings["invert_time"]);

    if(Settings.contains("use_audio"))
        ui->use_audio->setChecked(Settings["use_audio"]);

    if (Settings.contains("audio_settings"))
    {
        audio_settings_struct = Settings["audio_settings"];
        audio_settings.SetSettings(&audio_settings_struct);
    }
}

json Shaders::SaveCustomSettings()
{
    json settings;

    settings["shader_name"]      = ui->shaders->currentText().toStdString();
    settings["shader_program"]   = shader_renderer->Program()->ToJSON();
    settings["width"]            = width;
    settings["height"]           = height;
    settings["show_rendering"]   = show_rendering;
    settings["invert_time"]      = invert_time;
    settings["use_audio"]        = use_audio;
    settings["audio_settings"] = audio_settings_struct;

    return settings;
}

void Shaders::StartAudio()
{
    if(audio_settings_struct.audio_device >= 0)
    {
        AudioManager::get()->RegisterClient(audio_settings_struct.audio_device, this);
    }
}

void Shaders::StopAudio()
{
    if(audio_settings_struct.audio_device >= 0)
    {
        AudioManager::get()->UnRegisterClient(audio_settings_struct.audio_device, this);
    }
}

void Shaders::on_show_rendering_stateChanged(int state)
{
    ui->preview->setVisible(state);
    show_rendering = state;
}

void Shaders::on_use_audio_stateChanged(int state)
{
    ui->audio_settings->setVisible(state);

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

void Shaders::OnAudioDeviceChanged(int value)
{
    if(!use_audio)
    {
        audio_settings_struct.audio_device = value;
        return;
    }

    bool was_running = EffectEnabled;

    if(EffectEnabled)
    {
        StopAudio();
    }

    audio_settings_struct.audio_device = value;

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

    ShaderProgram* program = new ShaderProgram();
    program->main_pass->data.fragment_shader = frag_in.readAll().toStdString();
    program->Resize(width, height);
    shader_renderer->SetProgram(program);
    editor->SetProgram(program);
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

void Shaders::on_invert_time_stateChanged(int value)
{
    invert_time = value;
}

void Shaders::on_edit_clicked()
{
    editor->show();
}
void Shaders::on_time_reset_clicked()
{
    time = 0.f;
}

void Shaders::on_audio_settings_clicked()
{
    audio_settings.show();
}
