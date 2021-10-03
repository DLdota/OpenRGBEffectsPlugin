#include "ShaderProgram.h"

ShaderProgram::ShaderProgram()
{    
    main_pass = new ShaderPass(ShaderPass::BUFFER);
}

ShaderProgram::~ShaderProgram()
{
    passes.clear();

    delete main_pass;
}

void ShaderProgram::Init()
{
    for(ShaderPass* pass: passes)
    {
        pass->Init(width, height);
    }

    main_pass->Init(width, height);

    initialized = true;
}

QString ShaderProgram::Compile()
{
    QString log = main_pass->Recompile(version);

    for(ShaderPass* pass: passes)
    {
        log += pass->Recompile(version);
    }

    recompile = false;

    return log;
}

void ShaderProgram::Draw(const Uniforms& uniforms, QOpenGLFunctions* gl)
{
    GLuint units[4] = {GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE2, GL_TEXTURE3};
    int i = 0;

    for(ShaderPass* pass: passes)
    {
        if(resize)
        {
            pass->Resize(width, height);
        }

        pass->Draw(uniforms, units[i++], gl);
    }

    if(resize)
    {
        main_pass->Resize(width, height);
    }

    main_pass->Draw(uniforms, GL_TEXTURE4, gl);

    resize = false;
}

QImage ShaderProgram::Image()
{
    return main_pass->toImage();
}

void ShaderProgram::Resize(int width, int height)
{
    this->width = width;
    this->height = height;

    resize = true;
}

void ShaderProgram::SetVersion(std::string version)
{
    this->version = version;
}

std::string ShaderProgram::GetVersion()
{
    return version;
}

ShaderProgram* ShaderProgram::Copy()
{
    ShaderProgram* copy = new ShaderProgram();

    copy->main_pass = main_pass->Copy();

    copy->version = version;
    copy->width = width;
    copy->height = height;

    for(ShaderPass* pass: passes)
    {
        copy->passes.push_back(pass->Copy());
    }

    return copy;

}

ShaderProgram* ShaderProgram::FromJSON(json j)
{
    ShaderProgram* prog = new ShaderProgram();

    prog->main_pass = ShaderPass::FromJSON(j["main_pass"]);

    prog->version = j["version"];
    prog->width = j["width"];
    prog->height = j["height"];

    for(json pass_json: j["passes"])
    {
        prog->passes.push_back(ShaderPass::FromJSON(pass_json));
    }

    return prog;
}

json ShaderProgram::ToJSON()
{
    json j;

    j["main_pass"] = main_pass->ToJSON();
    j["version"] = version;
    j["width"] = width;
    j["height"] = height;

    std::vector<json> passes_json;

    for(ShaderPass* pass: passes)
    {
        passes_json.push_back(pass->ToJSON());
    }

    j["passes"] = passes_json;

    return j;
}
