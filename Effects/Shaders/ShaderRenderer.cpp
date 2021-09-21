#include "ShaderRenderer.h"

ShaderRenderer::ShaderRenderer(QObject *parent) :
    QObject(parent),
    clock(new std::chrono::steady_clock())
{}

void ShaderRenderer::Start()
{
    Stop();

    recompile_shader = true;
    running = true;
    thread = new std::thread(&ShaderRenderer::RendererThreadFunction, this);
}

void ShaderRenderer::Stop()
{
    if(thread != nullptr)
    {
        running = false;
        thread->join();
        delete thread;
        thread = nullptr;
    }
}

void ShaderRenderer::SetFPS(int value)
{
    FPS = value;
}

void ShaderRenderer::RendererThreadFunction()
{
    surface = new QOffscreenSurface();
    surface->create();

    context = new QOpenGLContext();
    context->setFormat(surface->format());
    context->create();
    context->makeCurrent(surface);

    QOpenGLFunctions* functions = context->functions();

    std::string vendor = reinterpret_cast<const char*>(functions->glGetString(GL_VENDOR));
    std::string renderer = reinterpret_cast<const char*>(functions->glGetString(GL_RENDERER));
    std::string version = reinterpret_cast<const char*>(functions->glGetString(GL_VERSION));

    printf("[OpenRGBEffectsPlugin] OpenGL vendor: %s, renderer: %s, version: %s\n", vendor.c_str(), renderer.c_str(), version.c_str());

    surface->setFormat(context->format());

    fbo = new QOpenGLFramebufferObject(width, height);

    program = new QOpenGLShaderProgram();

    //program->addShaderFromSourceCode(QOpenGLShader::Vertex, MakeVertexShader().c_str());
    program->link();

    //qDebug() << "program linked " << program->isLinked();

    while(running)
    {
        TCount start = clock->now();

        update();

        TCount end = clock->now();

        int duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        int FPSDelay = 1000000 / (float)FPS;
        int delta = FPSDelay - duration;

        if(delta > 0)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(delta));
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    // clean
    delete fbo;
    delete surface;
    delete context;
    delete program;
}

void ShaderRenderer::update()
{
    if(resize)
    {
        delete fbo;
        fbo = new QOpenGLFramebufferObject(width, height);
        resize = false;
    }

    if(recompile_shader && !shader.empty())
    {
        program->link();
        program->removeAllShaders();

        program->addShaderFromSourceCode(QOpenGLShader::Vertex, MakeVertexShader().c_str());
        program->addShaderFromSourceCode(QOpenGLShader::Fragment, MakeFragmentShader(shader).c_str());

        emit Log(program->log());

        recompile_shader = false;
    }

    program->bind();
    fbo->bind();

    program->setUniformValue("iTime", data.iTime);
    program->setUniformValueArray("iAudio", data.iAudio, 256, 1);
    program->setUniformValue("iResolution", QVector3D(width, height, 1));

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

    glBindTexture(GL_TEXTURE_2D, fbo->texture());

    emit Image(fbo->toImage());
}

ShaderRenderer::~ShaderRenderer()
{
    Stop();
}

void ShaderRenderer::Update(const ShaderData& data)
{
    this->data = data;
}

void ShaderRenderer::SetShader(std::string shader)
{
    this->shader = shader;
    recompile_shader = true;
}

void ShaderRenderer::Resize(int width, int height)
{
    this->width = width;
    this->height = height;

    resize = true;
}

std::string ShaderRenderer::MakeVertexShader()
{
    return  "attribute highp vec4 vertices;\n"
            "varying highp vec2 frag_coords;\n"
            "void main() {\n"
                "gl_Position = vec4(vertices.xy,0.0,1.0);\n"
            "}\n";
}

std::string ShaderRenderer::MakeFragmentShader(std::string shader)
{
    std::string header =
            "#define version 400\n"
            "#define HW_PERFORMANCE 1 \n"
            "uniform vec3      iResolution;\n"
            "uniform float     iTime;\n"
            "uniform float     iAudio[256];\n"
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
