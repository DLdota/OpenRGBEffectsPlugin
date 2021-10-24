#ifndef COLORUTILS_H
#define COLORUTILS_H

#include "RGBController.h"
#include "hsv.h"
#include <QColor>

enum ColorBlendFn
{
    MULTIPLY = 0,
    SCREEN = 1,
    OVERLAY = 2,
    DODGE = 3,
    BURN = 4,
    MASK = 5,
    LIGHTEN = 6,
    DARKEN = 7,
    EXCLUSIVE = 8,
    DIFFERENCE = 9
};

static std::vector<std::string> COLOR_BLEND_FN_NAMES = {
    "Multiply",
    "Screen",
    "Overlay",
    "Dodge",
    "Burn",
    "Mask",
    "Lighten",
    "Darken",
    "Exclusive",
    "Difference"
};

class ColorUtils {

public:

    static hsv_t RandomHSVColor()
    {
        hsv_t hsv;

        hsv.hue = rand() % 360;
        hsv.saturation = 255;
        hsv.value = 255;

        return hsv;
    }

    static RGBColor RandomRGBColor()
    {
        hsv_t hsv = RandomHSVColor();
        return RGBColor(hsv2rgb(&hsv));
    }

    static RGBColor Enlight(RGBColor color, float value)
    {
        hsv_t hsv;
        rgb2hsv(color, &hsv);

        hsv.value *= value;

        return RGBColor(hsv2rgb(&hsv));
    }

    static RGBColor Saturate(RGBColor color, float value)
    {
        hsv_t hsv;
        rgb2hsv(color, &hsv);

        hsv.saturation *= value;

        return RGBColor(hsv2rgb(&hsv));
    }

    static RGBColor Invert(RGBColor color)
    {
        int r = (255-RGBGetRValue(color));
        int g = (255-RGBGetGValue(color));
        int b = (255-RGBGetBValue(color));

        return ToRGBColor(r, g, b);
    }

    static RGBColor Interpolate(RGBColor color1, RGBColor color2, float fraction)
    {
        return ToRGBColor(
                    InterpolateChanel(RGBGetRValue(color1), RGBGetRValue(color2), fraction),
                    InterpolateChanel(RGBGetGValue(color1), RGBGetGValue(color2), fraction),
                    InterpolateChanel(RGBGetBValue(color1), RGBGetBValue(color2), fraction)
                    );
    }

    static RGBColor Multiply(RGBColor color1, RGBColor color2)
    {
        return ToRGBColor(
                    MultiplyChanel(RGBGetRValue(color1), RGBGetRValue(color2)),
                    MultiplyChanel(RGBGetGValue(color1), RGBGetGValue(color2)),
                    MultiplyChanel(RGBGetBValue(color1), RGBGetBValue(color2))
                    );
    };

    static RGBColor Screen(RGBColor color1, RGBColor color2)
    {
        return ToRGBColor(
                    ScreenChanel(RGBGetRValue(color1), RGBGetRValue(color2)),
                    ScreenChanel(RGBGetGValue(color1), RGBGetGValue(color2)),
                    ScreenChanel(RGBGetBValue(color1), RGBGetBValue(color2))
                    );
    };

    static RGBColor Overlay(RGBColor color1, RGBColor color2)
    {
        return ToRGBColor(
                    OverlayChanel(RGBGetRValue(color1), RGBGetRValue(color2)),
                    OverlayChanel(RGBGetGValue(color1), RGBGetGValue(color2)),
                    OverlayChanel(RGBGetBValue(color1), RGBGetBValue(color2))
                    );
    };

    static RGBColor Dodge(RGBColor color1, RGBColor color2)
    {
        return ToRGBColor(
                    DodgeChanel(RGBGetRValue(color1), RGBGetRValue(color2)),
                    DodgeChanel(RGBGetGValue(color1), RGBGetGValue(color2)),
                    DodgeChanel(RGBGetBValue(color1), RGBGetBValue(color2))
                    );
    };

    static RGBColor Burn(RGBColor color1, RGBColor color2)
    {
        return ToRGBColor(
                    BurnChanel(RGBGetRValue(color1), RGBGetRValue(color2)),
                    BurnChanel(RGBGetGValue(color1), RGBGetGValue(color2)),
                    BurnChanel(RGBGetBValue(color1), RGBGetBValue(color2))
                    );
    };

    static RGBColor Lighten(RGBColor color1, RGBColor color2)
    {
        return ToRGBColor(
                    LightenChanel(RGBGetRValue(color1), RGBGetRValue(color2)),
                    LightenChanel(RGBGetGValue(color1), RGBGetGValue(color2)),
                    LightenChanel(RGBGetBValue(color1), RGBGetBValue(color2))
                    );
    };

    static RGBColor Darken(RGBColor color1, RGBColor color2)
    {
        return ToRGBColor(
                    DarkenChanel(RGBGetRValue(color1), RGBGetRValue(color2)),
                    DarkenChanel(RGBGetGValue(color1), RGBGetGValue(color2)),
                    DarkenChanel(RGBGetBValue(color1), RGBGetBValue(color2))
                    );
    };    

    static RGBColor Exclusive(RGBColor color1, RGBColor color2)
    {
        return color2 > 0 ? color2 : color1;
    };

    static RGBColor Difference(RGBColor color1, RGBColor color2)
    {
        return ToRGBColor(
                    DifferenceChanel(RGBGetRValue(color1), RGBGetRValue(color2)),
                    DifferenceChanel(RGBGetGValue(color1), RGBGetGValue(color2)),
                    DifferenceChanel(RGBGetBValue(color1), RGBGetBValue(color2))
                    );
    };

    static RGBColor ApplyColorBlendFn(RGBColor c1, RGBColor c2, ColorBlendFn fn)
    {
        switch (fn)
        {
        case MULTIPLY: return Multiply(c1, c2);
        case SCREEN:   return Screen(c1, c2);
        case OVERLAY:  return Overlay(c1, c2);
        case DODGE:    return Dodge(c1, c2);
        case BURN:     return Burn(c1, c2);
        case MASK:     return Mask(c1, c2);
        case LIGHTEN:  return Lighten(c1, c2);
        case DARKEN:   return Darken(c1, c2);
        case EXCLUSIVE:   return Exclusive(c1, c2);
        case DIFFERENCE:   return Difference(c1, c2);
        default:       return OFF();
        }
    }

    static RGBColor Mask(RGBColor color1, RGBColor color2)
    {
        return color2 > 0 ? color1 : 0;
    };

    static RGBColor OFF()
    {
        return ToRGBColor(0,0,0);
    }

    static RGBColor fromQColor(QColor c)
    {
        return ToRGBColor(c.red(), c.green(), c.blue());
    }

    static QColor toQColor(RGBColor c)
    {
        return QColor(RGBGetRValue(c), RGBGetGValue(c), RGBGetBValue(c));
    }

    static RGBColor apply_brightness(RGBColor color, float brightness)
    {
        return ToRGBColor(
                    (int)(RGBGetRValue(color) * brightness),
                    (int)(RGBGetGValue(color) * brightness),
                    (int)(RGBGetBValue(color) * brightness)
                    );
    }

private:

    static unsigned char InterpolateChanel(unsigned char a, unsigned char b, float x)
    {
        return (int) ((b - a) * x + a);
    }

    static unsigned char MultiplyChanel(unsigned char a, unsigned char b)
    {
        return b * a / 255;
    }

    static unsigned char ScreenChanel(unsigned char a, unsigned char b)
    {
        return 255 - ((255 - b) * (255 - a) >> 8);;
    }

    static unsigned char OverlayChanel(unsigned char a, unsigned char b)
    {
        return 128 > a ? 2 * b * a / 255 : 255 - 2 * (255 - a) * (255 - b) / 255;
    }

    static unsigned char DodgeChanel(unsigned char a, unsigned char b)
    {
        return 255 == a ? a : std::min<int>(255, (b << 8) / (255 - a));
    }

    static unsigned char BurnChanel(unsigned char a, unsigned char b)
    {
        return 0 == a ? a : std::max<int>(0, 255 - ((255 - b) << 8) / a);
    }

    static unsigned char MaskChanel(unsigned char a, unsigned char b)
    {
        return b > 0 ? a : 0;
    }

    static unsigned char LightenChanel(unsigned char a, unsigned char b)
    {
        return std::max<int>(a, b);
    }

    static unsigned char DarkenChanel(unsigned char a, unsigned char b)
    {
        return std::min<int>(a, b);
    }

    static unsigned char DifferenceChanel(unsigned char a, unsigned char b)
    {
        return std::max<int>(a - b, 0);
    }

};

#endif // COLORUTILS_H
