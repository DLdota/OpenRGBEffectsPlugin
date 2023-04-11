#ifndef RECTANGLESELECTOR_H
#define RECTANGLESELECTOR_H

#include <QApplication>
#include <QMouseEvent>
#include <QScreen>
#include <QWidget>
#include <QPainter>
#include <QDialog>
#include <memory>

class RectangleSelector : public QWidget {
    Q_OBJECT

public:
    RectangleSelector(QWidget* parent);

    void SetRectangle(const QRect& r);

private:
    void UpdateMask();

    QPixmap texture;

    static constexpr int border = 2;

    static QRect MapToLogicalCoordinates(const QRect&);
    static std::vector<QRect> GetScreenGeometries();
    static QRect CombineScreenGeometries(const std::vector<QRect>&);
    static QRect ValidateRubberBandRectangle(QRect);\


protected:
    virtual void resizeEvent(QResizeEvent* event) override;
    virtual void paintEvent(QPaintEvent* event) override;

};


class RectangleSelectorOverlay: public QWidget
{
    Q_OBJECT

public:
    explicit RectangleSelectorOverlay(QWidget *parent = nullptr);
    void StartSelection(int screen_index);
    QWidget qw;

signals:
    void SelectionUpdated(QRect);

protected:
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;

private:
    bool selecting_area = false;
    Qt::WindowFlags original_flags;

    QScreen* screen;

    std::unique_ptr<RectangleSelector> rectangle_selector;
    QRect selection_rect;
    QRect MapToCurrentScreen(QRect);
    void UpdateSelection();
    void StopSelection();

};


#endif // RECTANGLESELECTOR_H

