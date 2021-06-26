#include "PreviewWidget.h"

void PreviewWidget::mouseDoubleClickEvent(QMouseEvent*)
{
    ToggleFullScreen();
}

void PreviewWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_F11)
    {
        ToggleFullScreen();
    }
    else if(event->key() == Qt::Key_Escape)
    {
        setWindowFlags(original_flags);
        showNormal();
    }
}

void PreviewWidget::ToggleFullScreen()
{
    if(isFullScreen())
    {
        setWindowFlags(original_flags);
        showNormal();
    }
    else
    {
        setWindowFlags(Qt::Window);        
        showFullScreen();
    }
}
