#ifndef COLORUTILS_H
#define COLORUTILS_H

#include "RGBController.h"
#include "hsv.h"

class ColorUtils {

public:

    static RGBColor Interpolate(RGBColor color1, RGBColor color2, float fraction)
    {
        unsigned char   r1 = RGBGetRValue(color1);
        unsigned char   r2 = RGBGetRValue(color2);
        unsigned char   g1 = RGBGetGValue(color1);
        unsigned char   g2 = RGBGetGValue(color2);
        unsigned char   b1 = RGBGetBValue(color1);
        unsigned char   b2 = RGBGetBValue(color2);

        return RGBColor(ToRGBColor(
                            (int) ((r2 - r1) * fraction + r1),
                            (int) ((g2 - g1) * fraction + g1),
                            (int) ((b2 - b1) * fraction + b1)
                            ));
    }

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

    static RGBColor Invert(RGBColor color)
    {
        return ToRGBColor(
            (255-RGBGetRValue(color)),
            (255-RGBGetGValue(color)),
            (255-RGBGetBValue(color))
        );
    }

    static RGBColor Enlight(RGBColor color, float brightness)
    {
        hsv_t hsv;
        rgb2hsv(color, &hsv);
        hsv.value *= brightness;
        return RGBColor(hsv2rgb(&hsv));
    }

    static RGBColor Screen(RGBColor color1, RGBColor color2)
    {
        unsigned char   r1 = RGBGetRValue(color1);
        unsigned char   r2 = RGBGetRValue(color2);
        unsigned char   g1 = RGBGetGValue(color1);
        unsigned char   g2 = RGBGetGValue(color2);
        unsigned char   b1 = RGBGetBValue(color1);
        unsigned char   b2 = RGBGetBValue(color2);

        unsigned char   r = 255 - ((255 - r2) * (255 - r1) >> 8);
        unsigned char   g = 255 - ((255 - g2) * (255 - g1) >> 8);
        unsigned char   b = 255 - ((255 - b2) * (255 - b1) >> 8);

        return RGBColor(ToRGBColor(r, g, b));
    };

};

#endif // COLORUTILS_H
