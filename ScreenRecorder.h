#ifndef SCREENRECORDER_H
#define SCREENRECORDER_H

#include <QGuiApplication>
#include <QScreen>
#include <QPixmap>
#include <QImage>
#include <thread>
#include <map>
#include <set>

class ScreenRecorder
{
public:
    ScreenRecorder();
    ~ScreenRecorder();

    QImage Capture();

    void Start();
    void Stop();

    void SetScreen(int screen_index);
    void SetRect(QRect rect);

private:
    std::thread* capture_thread = nullptr;

    bool continue_capture = false;
    void CaptureThreadFunction();

    QScreen* screen = nullptr;
    QRect rect;
    QImage capture;
};

#endif // SCREENRECORDER_H
