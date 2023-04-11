#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

#include <QKeyEvent>
#include <QMouseEvent>
#include <QLabel>

class PreviewWidget : public QLabel
{
     Q_OBJECT

public:
    explicit PreviewWidget(QWidget* parent = nullptr): QLabel(parent), original_flags(windowFlags()){};
    void mouseDoubleClickEvent(QMouseEvent*) override;
    void keyPressEvent(QKeyEvent*) override;

private:
    void ToggleFullScreen();
    Qt::WindowFlags original_flags;
};

#endif // PREVIEWWIDGET_H
