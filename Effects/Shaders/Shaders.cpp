#include "Shaders.h"

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

        ui->preview->setPixmap(QPixmap::fromImage(image));
        image_mutex.unlock();
    });

    connect(shader_renderer, &ShaderRenderer::Log, editor, &GLSLCodeEditor::SetLog);

    /*-----------------------------------------------*\
    | List the embeded shadders, add them to the      |
    | combo box                                       |
    \*-----------------------------------------------*/
    QDirIterator it(":/shaders");

    while (it.hasNext())
    {
        QString path = it.next();
        shader_paths.push_back(path);
        ui->shaders->addItem(path.split( "/" ).last());
    }
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
    }
    else
    {
        shader_renderer->Stop();
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

    shader_renderer->SetITime(0.f);
    Apply();
}


void Shaders::Apply()
{    
    std::string fragmentShader = editor->GetContent();
    shader_renderer->SetShader(fragmentShader);
}

void Shaders::Resize()
{
    shader_renderer->Resize(width, height);
}

void Shaders::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    iTime += 0.001 * Speed / (float) FPS;

    image_mutex.lock();

    shader_renderer->SetITime(iTime);

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

    if(Settings.contains("width"))
        ui->width->setValue(Settings["width"]);

    if(Settings.contains("height"))
        ui->height->setValue(Settings["height"]);

    Apply();
}

json Shaders::SaveCustomSettings(json Settings)
{
    Settings["shader_name"]    = ui->shaders->currentText().toStdString();
    Settings["fragmentShader"] = editor->GetContent();
    Settings["width"]          = width;
    Settings["height"]         = height;

    return Settings;
}

void Shaders::on_show_rendering_clicked()
{
    ui->preview->setVisible(ui->show_rendering->isChecked());
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
