#include "ShaderPass.h"
#include "ColorUtils.h"

ShaderPass::ShaderPass(Type type)
{
    printf("ShaderPass::ShaderPass (type=%d)\n", type);

    this->type = type;

    switch (type) {
    case BUFFER:
    {
        data.fragment_shader =
                "void mainImage( out vec4 fragColor, in vec2 fragCoord )\n"
                "{\n"
                    "  fragColor = vec4(1.0,0.0,0.0,1.0);\n"
                "}\n";
        break;
    }
    default: break;
    }
}

void ShaderPass::Init(int width, int height)
{
    printf("ShaderPass::Init\n");

    Resize(width, height);

    program = new QOpenGLShaderProgram();
    program->link();
}

void ShaderPass::Resize(int width, int height)
{
    printf("ShaderPass::Resize\n");

    this->width = width;
    this->height = height;

    if(fbo != nullptr)
    {
        delete fbo;
    }

    fbo = new QOpenGLFramebufferObject(width, height);
}

QString ShaderPass::Recompile(std::string version)
{
    printf("ShaderPass::Recompile\n");

    // re-link necessary??
    program->link();

    if(type == BUFFER)
    {
        program->removeAllShaders();

        program->addShaderFromSourceCode(
                    QOpenGLShader::Vertex,
                    MakeVertexShader().c_str());

        program->addShaderFromSourceCode(
                    QOpenGLShader::Fragment,
                    MakeFragmentShader(version, data.fragment_shader).c_str());
    }
    else if(type == TEXTURE)
    {
        program->removeAllShaders();

        program->addShaderFromSourceCode(
                    QOpenGLShader::Vertex,
                    MakeVertexShader().c_str());

        if(!data.texture_path.empty())
        {
            img = QImage(fbo->size(), QImage::Format_RGBA8888);
            img.load(QString::fromStdString(data.texture_path));
            texture = new QOpenGLTexture(
                        img.mirrored(false, true),
                        QOpenGLTexture::GenerateMipMaps
                        );
            texture->create();
        }
        else
        {
            printf("No texture path given\n");
        }

    }
    else if(type == AUDIO)
    {
        program->removeAllShaders();

        program->addShaderFromSourceCode(
                    QOpenGLShader::Vertex,
                    MakeVertexShader().c_str());

        img = QImage(512, 2, QImage::Format_RGBA8888);

        texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
        texture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
        texture->setSize(512, 2, 1);
        texture->setFormat(QOpenGLTexture::RGBA8_UNorm);
        texture->create();
    }

    return program->log();
}

void ShaderPass::Draw(const Uniforms& uniforms, GLenum unit, QOpenGLFunctions *gl)
{
    switch (type) {

    case BUFFER:
    {
        program->bind();
        fbo->bind();

        program->setUniformValue("iTime", uniforms.iTime);
        program->setUniformValueArray("iAudio", uniforms.iAudio, 256, 1);
        program->setUniformValue("iResolution", QVector3D(width, height, 1));
        program->setUniformValue("iMouse", QVector4D(0.,0.,0.,0.));
        program->setUniformValue("iChannel0", 0);
        program->setUniformValue("iChannel1", 1);
        program->setUniformValue("iChannel2", 2);
        program->setUniformValue("iChannel3", 3);

        program->enableAttributeArray(0);
        program->setAttributeBuffer(0, GL_FLOAT, 0, 3);

        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT);
        glBegin(GL_QUADS);
        glVertex2f(-1.0, -1.0);
        glVertex2f(-1.0, 1.0);
        glVertex2f(1.0, 1.0);
        glVertex2f(1.0, -1.0);
        glEnd();

        // ...

        fbo->release();
        program->release();

        gl->glActiveTexture(unit);
        glBindTexture(GL_TEXTURE_2D, fbo->texture());

        break;
    }

    case TEXTURE:
    {
        program->bind();
        fbo->bind();

        program->enableAttributeArray(0);
        program->setAttributeBuffer(0, GL_FLOAT, 0, 3);

        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT);
        glBegin(GL_QUADS);
        glVertex2f(-1.0, -1.0);
        glVertex2f(-1.0, 1.0);
        glVertex2f(1.0, 1.0);
        glVertex2f(1.0, -1.0);
        glEnd();

        fbo->release();
        program->release();

        if(!img.isNull())
        {
            gl->glActiveTexture(unit);
            glBindTexture(GL_TEXTURE_2D, texture->textureId());

        }

        break;
    }

    case AUDIO:
    {
        program->bind();
        fbo->bind();

        program->enableAttributeArray(0);
        program->setAttributeBuffer(0, GL_FLOAT, 0, 3);

        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT);
        glBegin(GL_QUADS);
        glVertex2f(-1.0, -1.0);
        glVertex2f(-1.0, 1.0);
        glVertex2f(1.0, 1.0);
        glVertex2f(1.0, -1.0);
        glEnd();

        fbo->release();
        program->release();

        if(uniforms.iAudio != nullptr)
        {
            // the sound texture is 512x2
            // first row is frequency data (48Khz/4 in 512 texels, meaning 23 Hz per texel)
            // second row is the sound wave, one texel is one mono sample

            for (int i = 0; i < 256; i++) {

                unsigned int a = (int)(uniforms.iAudio[i] * 255);
                RGBColor c = ToRGBColor(a,a,a);
                img.setPixel(2*i, 0, c);
                img.setPixel(2*i+1, 0, c);

                a = (int)(uniforms.iAudio[i] + 1.0) * 127;
                c = ToRGBColor(a,a,a);
                img.setPixel(2*i, 1, c);
                img.setPixel(2*i+1, 1, c);
            }

            texture->destroy();
            texture->create();
            texture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
            texture->setSize(512, 2, 1);
            texture->setFormat(QOpenGLTexture::R32F);
            texture->setData(img.mirrored(false, false));

            gl->glActiveTexture(unit);
            glBindTexture(GL_TEXTURE_2D, texture->textureId());
        }

        break;
    }

    default:
    {

        break;
    }

    }

}

std::string ShaderPass::MakeVertexShader()
{
    return  "attribute highp vec4 vertices;\n"
            "varying highp vec2 frag_coords;\n"
            "void main() {\n"
                "gl_Position = vec4(vertices.xy,0.0,1.0);\n"
            "}\n";
}

std::string ShaderPass::MakeFragmentShader(std::string pre_processor_version, std::string shader)
{
    std::string header =
            "#version " + pre_processor_version +  "\n"
            "#define HW_PERFORMANCE 1 \n"
            "uniform vec3      iResolution;\n"
            "uniform vec4      iMouse;\n"
            "uniform float     iTime;\n"
            "uniform float     iAudio[256];\n"
            "uniform sampler2D iChannel0;\n"
            "uniform sampler2D iChannel1;\n"
            "uniform sampler2D iChannel2;\n"
            "uniform sampler2D iChannel3;\n"
        ;


    std::string footer =
            "\n"
            "void main( void )\n"
              "{\n"
                "vec4 color = vec4(0.0,0.0,0.0,1.0);\n"
                "mainImage(color, gl_FragCoord.xy);\n"
                "gl_FragColor = vec4(color.xyz,1.0);\n"
            "}\n"
        ;

    return
            header +
            shader +
            footer ;
}

QImage ShaderPass::toImage()
{
    return fbo->toImage();
}

ShaderPass::~ShaderPass()
{
    printf("ShaderPass::~ShaderPass\n");

    if(fbo != nullptr)
    {
        delete fbo;
    }

    if(program != nullptr)
    {
        delete program;
    }
}

ShaderPass* ShaderPass::Copy()
{
    printf("ShaderPass::Copy\n");
    ShaderPass* copy = new ShaderPass(type);

    copy->data = data;

    return copy;
}

ShaderPass::Type ShaderPass::GetType()
{
    return type;
}

ShaderPass* ShaderPass::FromJSON(json j)
{
    ShaderPass* pass = new ShaderPass(j["type"]);

    pass->data.fragment_shader = j["fragment_shader"];
    pass->data.texture_path = j["texture_path"];

    return pass;
}

json ShaderPass::ToJSON()
{
    json j;

    j["type"] = type;
    j["fragment_shader"] = data.fragment_shader;
    j["texture_path"] = data.texture_path;

    return j;
}
