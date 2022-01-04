#ifndef SHADERS_H
#define SHADERS_H

#include <QWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <mutex>
#include <QFile>
#include <QMessageBox>
#include <QDirIterator>
#include "ui_Shaders.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"
#include "ShaderRenderer.h"
#include "GLSLCodeEditor.h"
#include "ColorUtils.h"
#include "chuck_fft.h"
#include "ShaderProgram.h"

namespace Ui {
class Shaders;
}

class Shaders : public RGBEffect
{
    Q_OBJECT

public:
    explicit Shaders(QWidget *parent = nullptr);
    ~Shaders();

    EFFECT_REGISTERER(ClassName(), CAT_SPECIAL, [](){return new Shaders;});

    static std::string const ClassName() {return "Shaders";}
    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone*>) override;
    void LoadCustomSettings(json) override;
    json SaveCustomSettings(json) override;

    void EffectState(bool) override;
    void SetFPS(unsigned int) override;


private slots:
    void on_show_rendering_stateChanged(int);
    void on_use_audio_stateChanged(int);
    void on_shaders_currentIndexChanged(int);
    void on_audio_device_currentIndexChanged(int);

    void on_width_valueChanged(int);
    void on_height_valueChanged(int);

    void on_invert_time_stateChanged(int);

    void on_amplitude_valueChanged(int);
    void on_decay_valueChanged(int);
    void on_average_valueChanged(int);

    void on_edit_clicked();
    void on_time_reset_clicked();

private:
    Ui::Shaders *ui;
    QImage image;

    ShaderRenderer* shader_renderer = nullptr;
    GLSLCodeEditor* editor = nullptr;
    GLSLHighlighter* highlighter = nullptr;

    float time = 0.f;

    unsigned int width = 128;
    unsigned int height = 128;

    void Resize();

    std::vector<QString> shader_paths;
    unsigned int current_shader_idx = 0;

    bool show_rendering = false;

    std::mutex image_mutex;

    // Audio stuff
    float fft[256];
    unsigned char buffer[256];
    float win_hanning[256];
    float fft_nrml[256];
    float fft_fltr[256] = { 0 };

    bool use_audio = false;

    bool invert_time = false;

    void StartAudio();
    void StopAudio();
    void HandleAudioCapture();

    int audio_device_idx = 0;
    float amplitude = 100.f;
    float decay = 85.f;
    float filter_constant = 1.f;
    unsigned int avg_size = 8;
};

#endif // SHADERS_H
