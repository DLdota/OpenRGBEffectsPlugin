#ifndef SCREENRECORDER_H
#define SCREENRECORDER_H

#include <QGuiApplication>
#include <QScreen>
#include <QPixmap>
#include <QImage>
#include <thread>
#include <map>
#include <set>
#include <mutex>

class ScreenRecorder
{
public:
    static ScreenRecorder* Get();

    ScreenRecorder();
    ~ScreenRecorder();

    const QImage Capture();

    void Start();
    void Stop();

    void SetScreen(int screen_index);
    void SetRect(QRect rect);

    void SetFpsCapture(int value);
    int GetFpsCapture();

private:
    std::thread* capture_thread = nullptr;

    static ScreenRecorder* instance;

    bool continue_capture = false;
    void CaptureThreadFunction();

    QScreen* screen = nullptr;
    QRect rect;
    QPixmap capture;

    std::mutex lock;
    int fpscapture = 60;

};

#endif // SCREENRECORDER_H
