#ifndef GIFPLAYER_H
#define GIFPLAYER_H

#include <QWidget>
#include <QMovie>

#include "ui_GifPlayer.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"


namespace Ui {
class GifPlayer;
}

class GifPlayer: public RGBEffect
{
    Q_OBJECT

public:
    explicit GifPlayer(QWidget *parent = nullptr);
    ~GifPlayer();

    EFFECT_REGISTERER(ClassName(), CAT_SPECIAL, [](){return new GifPlayer;});

    static std::string const ClassName() {return "GIFPlayer";}

    void StepEffect(std::vector<ControllerZone*>) override;

    void LoadCustomSettings(json) override;
    json SaveCustomSettings() override;

private:
    Ui::GifPlayer   *ui;
    QMovie *movie;
    QString fileName;

    void Load(QString);

private slots:
    void on_choose_gif_file_clicked();
};

#endif // WAVY_H
