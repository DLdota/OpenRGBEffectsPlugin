#ifndef LIVEPREVIEWCONTROLLER_H
#define LIVEPREVIEWCONTROLLER_H

#include <QWidget>
#include "RGBController.h"
#include "ui_LivePreviewController.h"

namespace Ui {
class LivePreviewController;
}

class LivePreviewController : public QWidget, public RGBController
{
    Q_OBJECT

public:
    explicit LivePreviewController(QWidget *parent = nullptr);
    ~LivePreviewController();

    // RGBController overrides
    void DeviceUpdateLEDs()    override;

    void SetupZones()          override {};
    void SetupColors()         override {};
    void ResizeZone(int, int)  override {};
    void SetCustomMode()       override {};
    void DeviceUpdateMode()    override {};
    void UpdateZoneLEDs(int)   override {};
    void UpdateSingleLED(int)  override {};

signals:
    void Rendered(QImage);
    void ReversedChanged(bool);

private slots:
    void on_presets_currentIndexChanged(int);
    void on_brightness_valueChanged(int);
    void on_width_valueChanged(int);
    void on_height_valueChanged(int);
    void on_reverse_stateChanged(int);
    void on_scale_stateChanged(int);
    void Draw(QImage);

private:
    Ui::LivePreviewController *ui;

    void SetupZone(std::string, zone_type, unsigned int, unsigned int);

    void Update(std::string, zone_type);

    struct ZonePreset{
        std::string name;
        zone_type zt;
        unsigned int height;
        unsigned int width;
    };

    std::vector<ZonePreset> presets = {
        {"Matrix 8x8", ZONE_TYPE_MATRIX, 8, 8},
        {"Matrix 8x16", ZONE_TYPE_MATRIX, 8, 16},
        {"Matrix 8x32", ZONE_TYPE_MATRIX, 8, 32},
        {"Matrix 8x64", ZONE_TYPE_MATRIX, 8, 64},
        {"Matrix 16x16", ZONE_TYPE_MATRIX, 16, 16},
        {"Matrix 16x32", ZONE_TYPE_MATRIX, 16, 32},
        {"Matrix 16x64", ZONE_TYPE_MATRIX, 16, 64},
        {"Matrix 32x32", ZONE_TYPE_MATRIX, 32, 32},
        {"Matrix 32x64", ZONE_TYPE_MATRIX, 32, 64},
        {"Matrix 64x64", ZONE_TYPE_MATRIX, 64, 64},
        {"Matrix 64x128", ZONE_TYPE_MATRIX, 64, 128},
        {"Matrix 128x128", ZONE_TYPE_MATRIX, 128, 128},
        {"Linear 8 LEDs", ZONE_TYPE_LINEAR, 8, 1},
        {"Linear 10 LEDs", ZONE_TYPE_LINEAR, 10, 1},
        {"Linear 12 LEDs", ZONE_TYPE_LINEAR, 12, 1},
        {"Linear 16 LEDs", ZONE_TYPE_LINEAR, 16, 1},
        {"Linear 20 LEDs", ZONE_TYPE_LINEAR, 20, 1},
        {"Linear 24 LEDs", ZONE_TYPE_LINEAR, 24, 1},
        {"Linear 48 LEDs", ZONE_TYPE_LINEAR, 48, 1},
        {"Linear 72 LEDs", ZONE_TYPE_LINEAR, 72, 1},
        {"Linear 128 LEDs", ZONE_TYPE_LINEAR, 128, 1}
    };

    unsigned int width = presets[0].width;
    unsigned int height = presets[0].height;


};

#endif // LIVEPREVIEWCONTROLLER_H
