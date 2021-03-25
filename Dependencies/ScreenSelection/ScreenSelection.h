#ifndef SCREENSELECTION_H
#define SCREENSELECTION_H

#include <QWidget>
#include <QVBoxLayout>
#include <QMainWindow>
#include <QMainWindow>
#include <QGuiApplication>
#include <QMainWindow>
#include <QScreen>
#include <QWindow>
#include <QLabel>
#include <QPainter>
#include <QResizeEvent>
#include <QStyleOption>
#include <QPainterPath>
#include <QRadioButton>
#include <QPushButton>

enum CALCULATION_TYPE
{
    CALCULATE_MODE,
    CALCULATE_AVERAGE
};


class ScreenSelection : public QWidget
{
    Q_OBJECT
public:
    explicit ScreenSelection(QWidget* Parent = nullptr);

    virtual QSize sizeHint () const;
    virtual QSize minimumSizeHint() const;

    void GetScreen();
    void SetAuto(bool Auto);

    QColor CalcColor();

    CALCULATION_TYPE GetCalcType();
    void             SetCalcType(CALCULATION_TYPE);

    bool             GetShowState();
    void             SetShowState(bool);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *);

private:
    QSpacerItem* ToBottom;

    QImage ToCalc;

    QPixmap ScreenShot;
    QPixmap ForMod;

    QScreen* screen;
    QRegion  imgRegion;

    QLabel* Display = nullptr;
    QSlider* WidthSlider;
    QSlider* HeightSlider;
    QSlider* XPosSlider;
    QSlider* YPosSlider;
    QRadioButton* AverageButton;
    QRadioButton* ModeButton;
    QPushButton* ShowHide;
    QPoint lastPos;

    bool Update;
    bool mouseDown;
    bool inRange;

    int XSize;
    int YSize;
    int XPos;
    int YPos;

    void drawIndicator();

    CALCULATION_TYPE CalcType = CALCULATE_AVERAGE;
    bool AlreadyDrawing = false;
    bool AutoRefresh = true;

private slots:
    void XPosChanged(const int NewX);
    void YPosChanged(const int NewY);
    void WidthChanged(const int NewWidth);
    void HeightChanged(const int NewHeight);

    void on_CALC_MODE();
    void on_CALC_AVERAGE();
    void on_ShowHide();
};



#endif // SCREENSELECTION_H
