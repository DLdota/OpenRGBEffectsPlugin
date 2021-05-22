#include "GifPlayer.h"
#include <QFileDialog>


REGISTER_EFFECT(GifPlayer);

GifPlayer::GifPlayer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GifPlayer),
    movie(nullptr)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "GifPlayer";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Use GIFs to create your own effect";

    EffectDetails.IsReversable = false;
    EffectDetails.MaxSpeed     = 0;
    EffectDetails.MinSpeed     = 0;
    EffectDetails.UserColors   = 0;

    EffectDetails.MaxSlider2Val = 0;
    EffectDetails.MinSlider2Val = 0;
    EffectDetails.Slider2Name   = "";

    EffectDetails.HasCustomWidgets = true;
    EffectDetails.HasCustomSettings = true;
}

GifPlayer::~GifPlayer()
{
    if(movie)
    {
        movie->stop();
        delete movie;
    }

    delete ui;
}

void GifPlayer::DefineExtraOptions(QLayout* layout)
{
    layout->addWidget(this);
}

void GifPlayer::StepEffect(std::vector<ControllerZone> controller_zones)
{
    if(!movie)
    {
        return;
    }

    QImage image = movie->currentImage();

    for(ControllerZone controller_zone : controller_zones)
    {

        if(controller_zone.type() == ZONE_TYPE_SINGLE || controller_zone.type() == ZONE_TYPE_LINEAR)
        {
            unsigned int width = controller_zone.leds_count();
            unsigned int height = 1;
            unsigned int start_idx = controller_zone.start_idx();

            QImage scaled = image.scaled(width, height);

            for(unsigned int i = 0; i < width; i++)
            {
                QColor color = scaled.pixelColor(i, 0);
                controller_zone.controller->SetLED(start_idx + i, ToRGBColor(color.red(), color.green(), color.blue()));
            }

        }
        else if(controller_zone.type() == ZONE_TYPE_MATRIX)
        {
            unsigned int width = controller_zone.matrix_map_width();
            unsigned int height = controller_zone.matrix_map_height();
            unsigned int * map = controller_zone.map();

            QImage scaled = image.scaled(width, height);

            for(unsigned int h = 0; h < height; h++)
            {
                for(unsigned int w = 0; w <  width; w++)
                {
                    QColor color = scaled.pixelColor(w, h);

                    unsigned int led_num = map[h * width + w];
                    controller_zone.controller->SetLED(led_num, ToRGBColor(color.red(), color.green(), color.blue()));
                }
            }

        }
    }
}

void GifPlayer::Load(QString file)
{
    if(!file.isEmpty())
    {
        fileName = file;
        movie = new QMovie(fileName);
        ui->movie->setMovie(movie);
        movie->start();
    }
}

void GifPlayer::on_choose_gif_file_clicked()
{
    Load(QFileDialog::getOpenFileName(this, tr("Open GIF file"), "", tr("GIF Files (*.gif)")));
}

void GifPlayer::LoadCustomSettings(json Settings)
{
    if (Settings.contains("fileName"))
    {
        Load(QString::fromStdString(Settings["fileName"]));
    }
}

json GifPlayer::SaveCustomSettings(json Settings)
{
    Settings["fileName"] = fileName.toStdString();
    return Settings;
}
