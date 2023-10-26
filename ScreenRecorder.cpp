#include "ScreenRecorder.h"
#include "OpenRGBEffectSettings.h"
#include <Windows.h>

ScreenRecorder* ScreenRecorder::instance;

ScreenRecorder::ScreenRecorder() {}

ScreenRecorder* ScreenRecorder::Get()
{
    if(!instance)
    {
        instance = new ScreenRecorder();
    }

    return instance;
}

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

    while(continue_capture)
    {
        int delay = 1000 / OpenRGBEffectSettings::globalSettings.fpscapture;
        if(screen == nullptr)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        auto start = std::chrono::steady_clock::now();

        lock.lock();
        capture = grabWindow(0, rect.left(), rect.top(), rect.width(), rect.height());
        lock.unlock();

        int took = (int) std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
        int delta = delay - took;

        // sleep 2 ms minimum
        std::this_thread::sleep_for(std::chrono::milliseconds(delta > 2 ? delta : 2));
    }

    printf("[OpenRGBEffectsPlugin] SCREENRECORDER: Thread stopped\n");
}

extern QPixmap qt_pixmapFromWinHBITMAP(HBITMAP bitmap, int format = 0);

QPixmap ScreenRecorder::grabWindow(quintptr window, int xIn, int yIn, int width, int height) const
{
    QSize windowSize;
    HWND hwnd = reinterpret_cast<HWND>(window);
    if (hwnd)
    {
        RECT r;
        GetClientRect(hwnd, &r);
        windowSize = QSize(r.right - r.left, r.bottom - r.top);
    }
    else
    {
        hwnd = GetDesktopWindow();
        const QRect screenGeometry = screen->geometry();
        windowSize = screenGeometry.size();
    }

    if (width < 0)
        width = windowSize.width() - xIn;

    if (height < 0)
        height = windowSize.height() - yIn;

    // Create and setup bitmap
    HDC display_dc = GetDC(nullptr);
    HDC bitmap_dc = CreateCompatibleDC(display_dc);
    HBITMAP bitmap = CreateCompatibleBitmap(display_dc, width, height);
    HGDIOBJ null_bitmap = SelectObject(bitmap_dc, bitmap);

    // copy data
    HDC window_dc = GetDC(hwnd);
    // remove CAPTUREBLT!!
    BitBlt(bitmap_dc, 0, 0, width, height, window_dc, xIn, yIn, SRCCOPY);

    // clean up all but bitmap
    ReleaseDC(hwnd, window_dc);
    SelectObject(bitmap_dc, null_bitmap);
    DeleteDC(bitmap_dc);
    const QPixmap pixmap = qt_pixmapFromWinHBITMAP(bitmap);
    DeleteObject(bitmap);
    ReleaseDC(nullptr, display_dc);

    return pixmap;
}

const QImage ScreenRecorder::Capture()
{
    lock.lock();
    QImage image = capture.toImage();
    lock.unlock();
    return image;
}
