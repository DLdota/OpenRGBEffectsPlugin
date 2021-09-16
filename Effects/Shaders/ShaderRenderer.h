#ifndef SHADERRENDERER_H
#define SHADERRENDERER_H

#include <QObject>
#include <QImage>
#include <QDebug>
#include <QOpenGLShaderProgram>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShader>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <thread>

typedef std::chrono::time_point<std::chrono::steady_clock,std::chrono::duration<long long, std::ratio<1,10000000000>>> TCount;

class ShaderRenderer : public QObject
{
    Q_OBJECT

public:
    explicit ShaderRenderer(QObject *parent = nullptr);
    ~ShaderRenderer();

    void SetITime(float);
    void SetShader(std::string);
    void Resize(int, int);
    void Start();
    void Stop();
    void SetFPS(int);

private slots:
    void update();

private:
    std::thread* thread = nullptr;
    void RendererThreadFunction();

    QOpenGLFramebufferObject* fbo = nullptr;
    QOpenGLShaderProgram* program = nullptr;
    QOffscreenSurface * surface = nullptr;
    QOpenGLContext * context = nullptr;

    float width = 128;
    float height = 128;
    float iTime = 0.f;

    std::string shader;

    QString last_log;

    bool resize = false;
    bool recompile_shader = false;

    int FPS = 60;

    std::string MakeFragmentShader(std::string);
    std::string MakeVertexShader();

    bool running = false;

    std::chrono::steady_clock* clock;

signals:
    void Image(const QImage&);
    void Log(QString);
};

#endif // SHADERRENDERER_H
