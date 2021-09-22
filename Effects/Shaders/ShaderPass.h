#ifndef SHADERPASS_H
#define SHADERPASS_H

#include <QDebug>
#include <QImage>
#include <QOpenGLShaderProgram>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShader>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <thread>
#include "ShaderPassData.h"
#include "json.hpp"

using json = nlohmann::json;

struct Uniforms
{
    float iTime = 0.f;
    float* iAudio = nullptr;
};

class ShaderPass
{
public:

    enum Type {
        TEXTURE,
        AUDIO,
        BUFFER
    };

    ShaderPass(Type);
    ~ShaderPass();

    // Must be called in an OpenGL context thread
    void Init(int, int);
    QString Recompile(std::string);
    void Draw(const Uniforms&, GLenum, QOpenGLFunctions*);
    void Resize(int,int);
    QImage toImage();

    ShaderPass* Copy();

    ShaderPass::Type GetType();

    ShaderPassData data;

    static ShaderPass* FromJSON(json);
    json ToJSON();


private:
    QOpenGLFramebufferObject* fbo = nullptr;
    QOpenGLShaderProgram* program = nullptr;

    std::string MakeVertexShader();
    std::string MakeFragmentShader(std::string,std::string);

    int width;
    int height;
    Type type;

    QImage img;

    QOpenGLTexture* texture = nullptr;
};

#endif // SHADERPASS_H
