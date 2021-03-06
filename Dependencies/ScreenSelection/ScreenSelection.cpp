#include "ScreenSelection.h"
#include "xtensor.hpp"
#include "xarray.hpp"
#include "xsort.hpp"
#include "xio.hpp"
#include "xadapt.hpp"
#include "RGBController.h"

ScreenSelection::ScreenSelection(QWidget* Parent) :
    QWidget(Parent)
{
    screen = QGuiApplication::primaryScreen();
    mouseDown = false;
    inRange = false;

    XPos = 0;
    YPos = 0;

    XSize = 10;
    YSize = 10;

    Display = new QLabel();
    Display->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    Display->setAlignment(Qt::AlignCenter);
    Display->setMinimumSize(this->minimumSize() / 2);

    const QRect ScreenSize = this->screen->geometry();

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(Display);

    /*------------*\
    | Size Sliders |
    \*------------*/
    QHBoxLayout *BoxSizeSliderLayout = new QHBoxLayout;

    QLabel* WidthWhatsThis = new QLabel("Width: ");
    BoxSizeSliderLayout->addWidget(WidthWhatsThis);
    WidthSlider = new QSlider();
    WidthSlider->setMaximum(ScreenSize.width());
    WidthSlider->setMinimum(1);
    WidthSlider->setOrientation(Qt::Horizontal);
    connect(WidthSlider,SIGNAL(valueChanged(int)),this,SLOT(WidthChanged(int)));
    BoxSizeSliderLayout->addWidget(WidthSlider);

    QLabel* HeightWhatsThis = new QLabel("Height: ");
    BoxSizeSliderLayout->addWidget(HeightWhatsThis);
    HeightSlider = new QSlider();
    HeightSlider->setMaximum(ScreenSize.height());
    HeightSlider->setMinimum(1);
    HeightSlider->setOrientation(Qt::Horizontal);
    connect(HeightSlider, SIGNAL(valueChanged(int)),this,SLOT(HeightChanged(int)));
    BoxSizeSliderLayout->addWidget(HeightSlider);


    ShowHide = new QPushButton();
    ShowHide->setText("Show Preview");
    connect(ShowHide,SIGNAL(clicked()),this,SLOT(on_ShowHide()));
    mainLayout->addWidget(ShowHide);

    /*-----------------*\
    | Position Sliders  |
    \*-----------------*/
    QHBoxLayout* PosSliderLayout = new QHBoxLayout;

    QLabel* XPosWhatsThis = new QLabel("X Pos: ");
    PosSliderLayout->addWidget(XPosWhatsThis);
    XPosSlider = new QSlider();
    XPosSlider->setMaximum(ScreenSize.width());
    XPosSlider->setMinimum(0);
    XPosSlider->setOrientation(Qt::Horizontal);
    connect(XPosSlider,SIGNAL(valueChanged(int)),this,SLOT(XPosChanged(int)));
    PosSliderLayout->addWidget(XPosSlider);

    QLabel* YPosWhatsThis = new QLabel("Y Pos: ");
    PosSliderLayout->addWidget(YPosWhatsThis);
    YPosSlider = new QSlider();
    YPosSlider->setMaximum(ScreenSize.height());
    YPosSlider->setMinimum(0);
    YPosSlider->setOrientation(Qt::Horizontal);
    connect(YPosSlider, SIGNAL(valueChanged(int)),this,SLOT(YPosChanged(int) ));
    PosSliderLayout->addWidget(YPosSlider);


    QFrame* RadioFrame = new QFrame();
    QHBoxLayout* RadioLayout = new QHBoxLayout(RadioFrame);


    RadioFrame->setLayout(RadioLayout);
    RadioFrame->setFixedHeight(40);
    RadioFrame->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);


    AverageButton = new QRadioButton("Average",RadioFrame);
    connect(AverageButton,SIGNAL(clicked()),this,SLOT(on_CALC_AVERAGE()));

    ModeButton    = new QRadioButton("Most Common",RadioFrame);
    connect(ModeButton,SIGNAL(clicked()),this,SLOT(on_CALC_MODE()));

    AverageButton->toggle();

    RadioLayout->addWidget(AverageButton);
    RadioLayout->addWidget(ModeButton);


    mainLayout->addLayout(BoxSizeSliderLayout);
    mainLayout->addLayout(PosSliderLayout);
    mainLayout->addWidget(RadioFrame);

    std::thread([=]()
    {
        while (true)
        {
            if (AutoRefresh)
            {
                GetScreen();
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::seconds(10));
            }
        }
    }).detach();

    GetScreen();
    ToCalc = ScreenShot.toImage();

    on_ShowHide();
    this->resize(this->minimumSize());
}

void ScreenSelection::GetScreen()
{
    QPixmap tmp = screen->grabWindow(0);

    if (Display == nullptr)
    {
        ScreenShot = tmp.scaled({(tmp.size().width()/50),(tmp.height()/50)},Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    else
    {
        QSize scaledSize = tmp.size();
        scaledSize.scale(Display->size(), Qt::KeepAspectRatio);
        if (Display->isHidden())
        {
            ScreenShot = tmp.scaled(Display->size(),Qt::KeepAspectRatio, Qt::FastTransformation);
            int TopLeft[2];
            TopLeft[0] = XPos;
            TopLeft[1] = YPos;

            int BottomRight[2];
            BottomRight[0] = XPos + XSize;
            BottomRight[1] = YPos + YSize;

            if ((XPos + XSize) > ScreenShot.width())
            {
                TopLeft[0] = ScreenShot.width() - XSize;
                BottomRight[0] = ScreenShot.width();
            }
            if ((YPos + YSize) > ScreenShot.height())
            {
                TopLeft[1] = ScreenShot.height() - YSize;
                BottomRight[1] = ScreenShot.height();
            }

            QPoint TopLeftPoint = QPoint(TopLeft[0],TopLeft[1]);
            QPoint BottomRightPoint = QPoint(BottomRight[0],BottomRight[1]);

            ToCalc = ScreenShot.copy(QRect(TopLeftPoint,BottomRightPoint)).toImage();
            return;
        }
        else
        {
            ScreenShot = tmp.scaled(Display->size(),Qt::KeepAspectRatio, Qt::SmoothTransformation);
            update();
        }
    }
}

void ScreenSelection::SetAuto(bool Auto)
{
    AutoRefresh = Auto;
}

QSize ScreenSelection::sizeHint() const
{
    return QSize(this->height(),this->width());
}

QSize ScreenSelection::minimumSizeHint() const
{
    return QSize(0,0);
}

void ScreenSelection::mousePressEvent(QMouseEvent *event)
{
    if (Update) Update = false;
    if (Display->isHidden()) return;

    lastPos = event->pos();

    if (imgRegion.contains(lastPos))
    {
        inRange = true;
        XPosSlider->setValue((lastPos.x() - (XSize/2)));
        YPosSlider->setValue((lastPos.y() - (YSize/2)));
        update();
    }

    mouseDown = true;
}

void ScreenSelection::mouseMoveEvent(QMouseEvent *event)
{
    if (Update) Update = false;
    if (Display->isHidden()) return;
    lastPos = event->pos();

    if (!mouseDown) return;

    if (imgRegion.contains(lastPos) && inRange)
    {
        inRange = true;
        XPosSlider->setValue((lastPos.x() - (XSize/2)));
        YPosSlider->setValue((lastPos.y() - (YSize/2)));
        update();
    }
}

void ScreenSelection::mouseReleaseEvent(QMouseEvent *)
{
    Update = true;
    mouseDown = false;
    inRange = false;
}

void ScreenSelection::resizeEvent(QResizeEvent*)
{
    this->layout()->setSizeConstraint(QLayout::SetMinAndMaxSize);
    QSize SZ = Display->size();

    imgRegion = QRegion(Display->pos().x(), Display->pos().y(),SZ.width(),SZ.height(), QRegion::Rectangle);

    QSize scaledSize = ScreenShot.size();
    scaledSize.scale(Display->size(), Qt::KeepAspectRatio);

    WidthSlider->setMaximum(scaledSize.width());
    XPosSlider->setMaximum(scaledSize.width());

    HeightSlider->setMaximum(scaledSize.height());
    YPosSlider->setMaximum(scaledSize.height());

    ScreenShot = ScreenShot.scaled(Display->size(),Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void ScreenSelection::drawIndicator()
{
    QPainter composePainter(&ForMod);

    int TopLeft[2];
    TopLeft[0] = XPos;
    TopLeft[1] = YPos;

    int BottomRight[2];
    BottomRight[0] = XPos + XSize;
    BottomRight[1] = YPos + YSize;

    if ((XPos + XSize) > ForMod.width())
    {
        TopLeft[0] = ForMod.width() - XSize;
        BottomRight[0] = ForMod.width();
    }
    if ((YPos + YSize) > ForMod.height())
    {
        TopLeft[1] = ForMod.height() - YSize;
        BottomRight[1] = ForMod.height();
    }

    QPoint TopLeftPoint = QPoint(TopLeft[0],TopLeft[1]);
    QPoint BottomRightPoint = QPoint(BottomRight[0],BottomRight[1]);

    ToCalc = ForMod.copy(QRect(TopLeftPoint,BottomRightPoint)).toImage();

    composePainter.drawRect(QRect(TopLeftPoint,BottomRightPoint));
    composePainter.end();
}

void ScreenSelection::paintEvent(QPaintEvent *)
{
    if (Display != nullptr) if (Display->isHidden()) return;
    if (AlreadyDrawing) return;

    ForMod = ScreenShot;

    AlreadyDrawing = true;

    QPainter painter(this);
    QStyleOption opt;
    opt.initFrom(this);
    drawIndicator();
    painter.drawPixmap(0,0,ForMod);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    AlreadyDrawing = false;
}

void ScreenSelection::XPosChanged(const int NewX)
{
    if (NewX < 0) XPos = 0;
    else XPos = NewX;

    if (Update) update();
}

void ScreenSelection::YPosChanged(const int NewY)
{
    if (NewY < 0) YPos = 0;
    else YPos = NewY;

    if (Update) update();
}

void ScreenSelection::WidthChanged(const int NewWidth)
{
    XSize = NewWidth;
    if (Update) update();
}

void ScreenSelection::HeightChanged(const int NewHeight)
{
    YSize = NewHeight;
    if (Update) update();
}

void ScreenSelection::on_CALC_MODE()
{
    if (CalcType == CALCULATE_AVERAGE)
    {
        CalcType = CALCULATE_MODE;
    }
}

void ScreenSelection::on_CALC_AVERAGE()
{
    if (CalcType == CALCULATE_MODE)
    {
        CalcType = CALCULATE_AVERAGE;
    }
}

void ScreenSelection::on_ShowHide()
{
    if (Display->isHidden())
    {
        QSize PrevSize = this->size();
        this->layout()->setSizeConstraint(QLayout::SetFixedSize);
        Display->show();
        ForMod = ScreenShot;
        ShowHide->setText("Hide Preview");
        update();
        this->layout()->setSizeConstraint(QLayout::SetDefaultConstraint);
        this->resize(PrevSize);
        this->adjustSize();
    }
    else
    {
        Display->hide();
        Display->clear();
        ForMod = QPixmap(0,0);
        ShowHide->setText("Show Preview");
        this->resize(this->minimumSize());
    }
}

QColor ScreenSelection::CalcColor()
{
    switch (CalcType)
    {
        case CALCULATE_AVERAGE:
        {
            int R = 0 , G = 0 , B = 0;
            for (int ColPX = 0; ColPX < ToCalc.width(); ColPX++)
            {
                for (int RowPX = 0; RowPX < ToCalc.height(); RowPX++)
                {
                    QColor QCol = ToCalc.pixelColor(ColPX,RowPX);
                    R += QCol.red();
                    G += QCol.green();
                    B += QCol.blue();
                }
            }
            int CR = (R / (ToCalc.width() * ToCalc.height()));
            int CG = (G / (ToCalc.width() * ToCalc.height()));
            int CB = (B / (ToCalc.width() * ToCalc.height()));

            return QColor(CR, CG, CB);
        }
        case CALCULATE_MODE:
        {
            QImage img = ToCalc;
            std::vector<uint> ToConvert;
            for (int ColPX = 0; ColPX < img.width(); ColPX++)
            {
                for (int RowPX = 0; RowPX < img.height(); RowPX++)
                {
                    QColor QCol = img.pixelColor(ColPX,RowPX);
                    ToConvert.push_back(ToRGBColor(QCol.red(),QCol.green(),QCol.blue()));
                }
            }

            xt::xarray<uint> ColArray = xt::adapt(ToConvert,{ToConvert.size(),(size_t)1});
            ColArray = xt::sort(ColArray);

            ToConvert.clear();

            RGBColor MostCommon = 0;
            RGBColor PrevColor;
            if (ColArray.size() > 0) PrevColor = ColArray[{0,0}];
            int HightestCount = 0;
            int CurrentCount = 0;
            for (int ColID = 0; ColID < (int)ColArray.size(); ColID++)
            {
                if (PrevColor == ColArray[{ColID , 0}]) CurrentCount += 1;
                else
                {
                    if (CurrentCount > HightestCount)
                    {
                        HightestCount = CurrentCount;
                        MostCommon = ColArray[{ColID , 0}];
                    }
                    CurrentCount = 0;
                }

                PrevColor = ColArray[{ColID , 0}];

            }

            if (CurrentCount > HightestCount){ MostCommon = ColArray[{CurrentCount , 0}]; };
            return QColor(RGBGetRValue(MostCommon),RGBGetGValue(MostCommon),RGBGetBValue(MostCommon));
        }
    }
    return QColor(255,255,255); // Appease the C++ gods and their stupid "Not all paths return a value" errors
}

CALCULATION_TYPE ScreenSelection::GetCalcType()
{
    return CalcType;
}

void ScreenSelection::SetCalcType(CALCULATION_TYPE CT)
{
    CalcType = CT;
    if (CT == CALCULATE_AVERAGE)
    {
        AverageButton->click();
    }
    else if (CT == CALCULATE_MODE)
    {
        ModeButton->click();
    }
}

bool ScreenSelection::GetShowState()
{
    return Display->isHidden();
}

void ScreenSelection::SetShowState(bool Showing)
{
    if (Display->isHidden() && Showing)
    {
        on_ShowHide();
    }
    else if (!Display->isHidden() && !Showing)
    {
        on_ShowHide();
    }
    else
    {
        return;
    }
}
