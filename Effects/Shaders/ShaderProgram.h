#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <vector>
#include "ShaderPass.h"
#include "json.hpp"

using json = nlohmann::json;

class ShaderProgram
{

public:
    ShaderProgram();
    ~ShaderProgram();

    void Init();
    QString Compile();
    void Draw(const Uniforms& uniforms, QOpenGLFunctions*);
    void SetVersion(std::string);
    std::string GetVersion();

    QImage Image();
    void Resize(int,int);

    ShaderProgram* Copy();

    ShaderPass* main_pass;
    std::vector<ShaderPass*> passes;

    bool recompile = false;
    bool resize = false;
    bool initialized = false;

    static ShaderProgram* FromJSON(json);
    json ToJSON();

private:

    std::string version = "110";
    int width = 128;
    int height = 128;
};

#endif // SHADERPROGRAM_H
