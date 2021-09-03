#include "ScreenRecorder.h"

ScreenRecorder::ScreenRecorder() {}

ScreenRecorder::~ScreenRecorder()
{
    continue_capture = false;

    if(capture_thread)
    {
        capture_thread->join();
        delete capture_thread;
    }
}

void ScreenRecorder::SetScreen(int screen_index)
{
    QList<QScreen*> screens = QGuiApplication::screens();

    if(screen_index >= 0 && screen_index < screens.size())
    {
        screen = screens[screen_index];
    }
    else
    {
        screen = nullptr;
    }
}

void ScreenRecorder::SetRect(QRect p_rect)
{
    rect = p_rect;
}

void ScreenRecorder::Start()
{
    if(capture_thread == nullptr)
    {
        continue_capture = true;
        capture_thread = new std::thread(&ScreenRecorder::CaptureThreadFunction, this);
    }
}

void ScreenRecorder::Stop()
{
    if(capture_thread != nullptr)
    {
        printf("[OpenRGBEffectsPlugin] SCREENRECORDER: Stopping capture thread...\n");

        continue_capture = false;
        capture_thread->join();
        delete capture_thread;
        capture_thread = nullptr;
    }
}

void ScreenRecorder::CaptureThreadFunction()
{
    printf("[OpenRGBEffectsPlugin] SCREENRECORDER: Thread started\n");

    int delay = 1000 / 60;

    while(continue_capture)
    {
        if(screen == nullptr)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        auto start = std::chrono::steady_clock::now();

        capture = screen->grabWindow(0, rect.left(), rect.top(), rect.width(), rect.height()).toImage();

        int took = (int) std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
        int delta = delay - took;

        // sleep 2 ms minimum
        std::this_thread::sleep_for(std::chrono::milliseconds(delta > 2 ? delta : 2));
    }

    printf("[OpenRGBEffectsPlugin] SCREENRECORDER: Thread stopped\n");
}

QImage ScreenRecorder::Capture()
{
    return capture;
}