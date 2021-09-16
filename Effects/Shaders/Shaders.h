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

namespace Ui {
class Shaders;
}

class Shaders : public RGBEffect
{
    Q_OBJECT

public:
    explicit Shaders(QWidget *parent = nullptr);
    ~Shaders();

    EFFECT_REGISTERER(ClassName(), [](){return new Shaders;});

    static std::string const ClassName() {return "Shaders";}
    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone*>) override;
    void LoadCustomSettings(json) override;
    json SaveCustomSettings(json) override;

    void EffectState(bool) override;
    void SetFPS(unsigned int) override;


private slots:
    void on_show_rendering_clicked();
    void on_shaders_currentIndexChanged(int);

    void on_width_valueChanged(int);
    void on_height_valueChanged(int);

    void on_edit_clicked();

private:
    Ui::Shaders *ui;
    float iTime = 0.f;
    QImage image;

    ShaderRenderer* shader_renderer = nullptr;
    GLSLCodeEditor* editor = nullptr;
    GLSLHighlighter* highlighter = nullptr;

    unsigned int width = 128;
    unsigned int height = 128;

    void Resize();
    void Apply();
    void LoadShader(std::string);    

    std::vector<QString> shader_paths;
    unsigned int current_shader_idx = 0;

    std::mutex image_mutex;
};

#endif // SHADERS_H
