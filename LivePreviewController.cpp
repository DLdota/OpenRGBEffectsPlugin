#include "LivePreviewController.h"
#include "OpenRGBPluginsFont.h"
#include <QScreen>

LivePreviewController::LivePreviewController(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LivePreviewController)
{
    ui->setupUi(this);

    name = "Preview controller";
    vendor = "OpenRGBEffectsPlugin";
    description = "Preview controller provided by OpenRGBEffectsPlugin";
    version = "1.0.0";
    serial = "EFFECTSPLUGIN_PREVEW_CONTROLLER_SERIAL";
    location = "Somewhere over the rainbow";
    active_mode = 0;
    type = DEVICE_TYPE_VIRTUAL;

    zones.resize(1);
    modes.resize(1);

    modes[0] = mode();
    zones[0] = zone();

    zones[0].matrix_map = nullptr;

    SetupZone("Matrix 64x64", ZONE_TYPE_MATRIX, 64, 64);

    modes[0].name = "Direct";
    modes[0].colors = colors;
    modes[0].value = 0;
    modes[0].flags = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_HAS_BRIGHTNESS;
    modes[0].brightness_min = 0;
    modes[0].brightness_max = 100;
    modes[0].brightness = 100;
    modes[0].color_mode = MODE_COLORS_PER_LED;

    connect(this, SIGNAL(Rendered(QImage)), this, SLOT(Draw(QImage)));

    for(ZonePreset& preset: presets)
    {
        ui->presets->addItem(QString::fromStdString(preset.name));
    }

    ui->preview_widget->setScaledContents(ui->scale->isChecked());

    ui->brightness->setValue(modes[0].brightness);

    ui->brightness_label->setFont(OpenRGBPluginsFont::GetFont());
    ui->brightness_label->setText(OpenRGBPluginsFont::icon(OpenRGBPluginsFont::sun));
}

LivePreviewController::~LivePreviewController()
{
    delete ui;

    if(zones[0].matrix_map != nullptr)
    {
        delete zones[0].matrix_map;
    }
}

void LivePreviewController::SetupZone(std::string name, zone_type zt, unsigned int width, unsigned int height)
{
    lock.lock();

    unsigned int size = width * height;

    colors.resize(size);
    leds.resize(size);

    zones[0].name = name;
    zones[0].leds_count = size;
    zones[0].leds_min = size;
    zones[0].leds_max = size;
    zones[0].start_idx = 0;
    zones[0].type = zt;

    zones[0].colors = &colors[0];
    zones[0].leds = &leds[0];

    modes[0].colors.resize(size);

    if(zt == ZONE_TYPE_LINEAR || zt == ZONE_TYPE_SINGLE)
    {
        for(unsigned int idx = 0 ; idx < size; idx++)
        {
            colors[idx] = 0;
        }

    }
    else if (zt == ZONE_TYPE_MATRIX)
    {
        if(zones[0].matrix_map != nullptr)
        {
            delete zones[0].matrix_map;
        }

        unsigned int *map = new unsigned int[height * width];

        zones[0].matrix_map = new matrix_map_type();
        zones[0].matrix_map->width = width;
        zones[0].matrix_map->height = height;
        zones[0].matrix_map->map = map;

        for(unsigned int h = 0; h < height; h++)
        {
            for(unsigned int w = 0; w < width; w++)
            {
                int idx = (h*width) + w;
                colors[idx] = 0;
                map[(h*width) + w] = idx;
            }
        }
    }

    lock.unlock();
}

void LivePreviewController::DeviceUpdateLEDs()
{
    lock.lock();

    zone_type zt = zones[0].type;

    float brightness = modes[0].brightness / 100.f;

    if(zt == ZONE_TYPE_LINEAR || zt == ZONE_TYPE_SINGLE)
    {
        QImage image(zones[0].leds_count, 1, QImage::Format_ARGB32);

        for(unsigned int idx = 0 ; idx < zones[0].leds_count; idx++)
        {
            int rgb = colors[idx];
            QColor color = QColor(RGBGetRValue(rgb) * brightness, RGBGetGValue(rgb) * brightness, RGBGetBValue(rgb) * brightness);
            image.setPixelColor(idx, 0, color);
        }
        emit Rendered(image);

    }
    else if (zt == ZONE_TYPE_MATRIX)
    {
        QImage image(zones[0].matrix_map->width, zones[0].matrix_map->height, QImage::Format_ARGB32);

        for(unsigned int h = 0; h<zones[0].matrix_map->height; h++)
        {
            for(unsigned  int w = 0; w < zones[0].matrix_map->width; w++)
            {
                int rgb = colors[(h*zones[0].matrix_map->width) + w];
                QColor color = QColor(RGBGetRValue(rgb) * brightness, RGBGetGValue(rgb) * brightness, RGBGetBValue(rgb) * brightness);
                image.setPixelColor(w, h, color);
            }
        }
        emit Rendered(image);
    }    

    lock.unlock();
}


void LivePreviewController::on_presets_currentIndexChanged(int value)
{
    ZonePreset& preset = presets[value];
    width = preset.width;
    height = preset.height;

    ui->width->blockSignals(true);
    ui->height->blockSignals(true);

    ui->width->setValue(width);
    ui->height->setValue(height);

    ui->width->blockSignals(false);
    ui->height->blockSignals(false);

    Update(preset.name, preset.zt);
}

void LivePreviewController::Draw(QImage image){
    if(ui->preview_widget->isFullScreen())
    {
        ui->preview_widget->setPixmap(QPixmap::fromImage(image));
    }
    else
    {
        ui->preview_widget->setPixmap(QPixmap::fromImage(
                                          image.scaled(ui->preview_widget->width(),ui->preview_widget->height(),
                                                       Qt::KeepAspectRatio, Qt::FastTransformation))
                                      );
    }
}

void LivePreviewController::Update(std::string name, zone_type zt)
{
    SetupZone(name, zt, width, height);
}

void LivePreviewController::on_width_valueChanged(int value)
{
    width = value;
    Update("Custom", ZONE_TYPE_MATRIX);
}

void LivePreviewController::on_height_valueChanged(int value)
{
    height = value;
    Update("Custom", ZONE_TYPE_MATRIX);
}

void LivePreviewController::on_reverse_stateChanged(int value)
{
    emit ReversedChanged(value);
}

void LivePreviewController::on_scale_stateChanged(int value)
{
    ui->preview_widget->setScaledContents(value);
}

void LivePreviewController::on_brightness_valueChanged(int value)
{
    modes[0].brightness = value;
}
