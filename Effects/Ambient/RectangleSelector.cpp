#include "RectangleSelector.h"
#include <math.h>
#include <QDesktopWidget>
#include <QBrush>

RectangleSelector::RectangleSelector(QWidget* parent)
    : QWidget(parent, Qt::Window | Qt::BypassWindowManagerHint | Qt::FramelessWindowHint |
              Qt::WindowStaysOnTopHint | Qt::WindowTransparentForInput | Qt::WindowDoesNotAcceptFocus)
{
    QImage image(16, 16, QImage::Format_RGB32);

    for(size_t j = 0; j < (size_t) image.height(); ++j)
    {
        uint32_t *row = (uint32_t*) image.scanLine(j);

        for(size_t i = 0; i < (size_t) image.width(); ++i)
        {
            row[i] = ((i + j) % 16 < 8)? 0xffff8080 : 0xff8080ff;
        }
    }

    texture = QPixmap::fromImage(image);

    UpdateMask();
}

void RectangleSelector::SetRectangle(const QRect& r)
{
    QRect rect = MapToLogicalCoordinates(ValidateRubberBandRectangle(r));

    if(rect.isEmpty())
    {
        hide();
    }
    else
    {
        setGeometry(rect);
        show();
    }
}

void RectangleSelector::UpdateMask()
{
    setMask(QRegion(0, 0, width(), height()).subtracted(QRegion(border, border, width() - 2 * border, height() - 2 * border)));
    setWindowOpacity(1.0);
}

void RectangleSelector::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    UpdateMask();
}

void RectangleSelector::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    texture.setDevicePixelRatio(devicePixelRatioF() / qApp->desktop()->devicePixelRatioF());

    painter.setPen(QColor(0, 0, 0, 128));
    painter.setBrush(Qt::NoBrush);
    painter.drawTiledPixmap(0, 0, width(), height(), texture);
    painter.drawRect(QRectF(0.5, 0.5, (qreal) width() - 1.0, (qreal) height() - 1.0));
}

QRect RectangleSelector::MapToLogicalCoordinates(const QRect& rect) {
    for(QScreen *screen :  QApplication::screens())
    {
        QRect geometry = screen->geometry();
        qreal ratio = screen->devicePixelRatio() / qApp->desktop()->devicePixelRatioF();
        QRect physical_geometry(geometry.x(), geometry.y(), std::lrint((qreal) geometry.width() * ratio), lrint((qreal) geometry.height() * ratio));

        if(physical_geometry.contains(rect.center()))
        {
            return QRect(
                        geometry.x() + std::lrint((qreal) (rect.x() - physical_geometry.x()) / ratio - 0.4999),
                        geometry.y() + std::lrint((qreal) (rect.y() - physical_geometry.y()) / ratio - 0.4999),
                        std::lrint((qreal) rect.width() / ratio - 0.4999),
                        std::lrint((qreal) rect.height() / ratio - 0.4999));
        }
    }

    return rect;
};

std::vector<QRect> RectangleSelector::GetScreenGeometries()
{
    std::vector<QRect> screen_geometries;

    for(QScreen *screen :  QApplication::screens())
    {
        QRect geometry = screen->geometry();
        qreal ratio = screen->devicePixelRatio() / qApp->desktop()->devicePixelRatioF();

        screen_geometries.emplace_back(geometry.x(), geometry.y(),
                                       std::lrint((qreal) geometry.width() * ratio),
                                       lrint((qreal) geometry.height() * ratio));
    }

    return screen_geometries;
}

QRect RectangleSelector::CombineScreenGeometries(const std::vector<QRect>& screen_geometries)
{
    QRect combined_geometry;

    for(const QRect &geometry : screen_geometries)
    {
        combined_geometry |= geometry;
    }

    return combined_geometry;
}

QRect RectangleSelector::ValidateRubberBandRectangle(QRect rect)
{
    std::vector<QRect> screen_geometries = GetScreenGeometries();
    QRect combined_geometry = CombineScreenGeometries(screen_geometries);
    return rect.normalized() & combined_geometry.adjusted(-10, -10, 10, 10);
}

RectangleSelectorOverlay::RectangleSelectorOverlay(QWidget *parent)
    : QWidget(parent), original_flags(windowFlags()){};

void RectangleSelectorOverlay::UpdateSelection() {
    if(rectangle_selector == NULL)
    {
        rectangle_selector.reset(new RectangleSelector(this));
    }
    rectangle_selector->SetRectangle(selection_rect);
}

void RectangleSelectorOverlay::mousePressEvent(QMouseEvent* event) {
    if(selecting_area)
    {
        if(event->button() == Qt::LeftButton)
        {
            QPoint mouse_global = {(int)event->screenPos().x(), (int)event->screenPos().y()};

            selection_rect = QRect(mouse_global, mouse_global);
            QRect normalized = MapToCurrentScreen(selection_rect.normalized());
            UpdateSelection();
            emit SelectionUpdated(normalized);
        }
        else
        {
            StopSelection();
        }

        event->accept();
        return;
    }
    event->ignore();
}

void RectangleSelectorOverlay::mouseMoveEvent(QMouseEvent* event) {
    if(selecting_area)
    {
        if(rectangle_selector != NULL)
        {
            QPoint mouse_global = {(int)event->screenPos().x(), (int)event->screenPos().y()};
            selection_rect.setBottomRight(mouse_global);
            QRect normalized = MapToCurrentScreen(selection_rect.normalized());
            emit SelectionUpdated(normalized);
            UpdateSelection();
        }

        event->accept();
        return;
    }

    event->ignore();
}

void RectangleSelectorOverlay::mouseReleaseEvent(QMouseEvent* event) {
    if(selecting_area)
    {
        if(event->button() == Qt::LeftButton)
        {
            if(rectangle_selector != NULL)
            {
                QRect normalized = MapToCurrentScreen(selection_rect.normalized());
                emit SelectionUpdated(normalized);
                StopSelection();
                event->accept();

                return;
            }
        }

        StopSelection();
        event->accept();

        return;
    }

    event->ignore();
}

QRect RectangleSelectorOverlay::MapToCurrentScreen(QRect rect)
{
    return QRect(
                rect.left() - screen->geometry().left(),
                rect.top()  - screen->geometry().top(),
                rect.width(),
                rect.height());
}


void RectangleSelectorOverlay::StartSelection(int screen_index)
{
    qw.setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::BypassWindowManagerHint);
    screen = QGuiApplication::screens()[screen_index];
    QSize size = screen->size();

    QRect screenres = screen->geometry();
    QPoint pos(screenres.left(), screenres.top());

    qw.resize(screenres.width(), screenres.height());
    qw.setMinimumSize(size);
    qw.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QPalette palette;
    palette.setBrush(QPalette::Window,(QBrush(screen->grabWindow(0, 0, 0, screenres.width(), screenres.height()))));
    qw.setPalette(palette);

    qw.show();

    move(pos);
    qw.move(pos);

    selecting_area = true;
    grabMouse(Qt::CrossCursor);
    setMouseTracking(true);
}


void RectangleSelectorOverlay::StopSelection()
{
    qw.setWindowFlags(original_flags);
    qw.hide();
    rectangle_selector.reset();
    setMouseTracking(false);
    releaseMouse();
    selecting_area = false;
    QWidget::activateWindow();
}
