#include "ShaderRenderer.h"

ShaderRenderer::ShaderRenderer(QObject *parent) :
    QObject(parent),
    shader_program(new ShaderProgram),
    clock(new std::chrono::steady_clock())
{}

void ShaderRenderer::Start()
{
    if(running)
    {
        return;
    }

    shader_program->initialized = false;
    shader_program->recompile = true;

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

    while(running)
    {
        TCount start = clock->now();

        // DRAW program
        program_lock.lock();

        if(!shader_program->initialized)
        {
            shader_program->Init();
        }

        if(shader_program->recompile)
        {
            emit Log(shader_program->Compile());
        }

        shader_program->Draw(uniforms, context->functions());

        emit Image(shader_program->Image());

        program_lock.unlock();
        // .....


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
    //delete shader_program;

    delete surface;
    delete context;
}


ShaderRenderer::~ShaderRenderer()
{
    Stop();

    if(shader_program != nullptr)
    {
        delete shader_program;
    }
}

ShaderProgram* ShaderRenderer::Program()
{
    return shader_program;
}

bool ShaderRenderer::isRunning()
{
    return running;
}

void ShaderRenderer::SetProgram(ShaderProgram* program)
{
    program_lock.lock();
    shader_program = program;
    shader_program->recompile = true;
    program_lock.unlock();
}

