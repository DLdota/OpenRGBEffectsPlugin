#define reverse true
#define animation_speed 1.
#define animation_direction false
#define color_rotation_speed 100.
#define color_rotation_direction false
#define center vec2(0.5)
#define spacing 1.
#define thickness 1.

vec4 hsv2rgb(float h, float s, float v)
    {
    float min;
    float chroma;
    float hPrime;
    float x;
    vec4 rgbColor;

    chroma = s * v;
    hPrime = h / 60.0;
    x = chroma * (1.0 - abs((mod(hPrime, 2.0) - 1.0)));

    if(hPrime < 1.0) {
            rgbColor.r = chroma;
            rgbColor.g = x;
    } else if(hPrime < 2.0) {
            rgbColor.r = x;
            rgbColor.g = chroma;
    } else if(hPrime < 3.0) {
            rgbColor.g = chroma;
            rgbColor.b = x;
    } else if(hPrime < 4.0) {
            rgbColor.g = x;
            rgbColor.b = chroma;
    } else if(hPrime < 5.0) {
            rgbColor.r = x;
            rgbColor.b = chroma;
    } else if(hPrime <= 6.0) {
            rgbColor.r = chroma;
            rgbColor.b = x;
    }

    min = v - chroma;

    rgbColor.r += min;
    rgbColor.g += min;
    rgbColor.b += min;
    rgbColor.a  = 1.0;

    return rgbColor;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord.xy / iResolution.xy;

    float animation_mult = 0.01 * (reverse ? -1.0 : 1.0) * animation_speed * (animation_direction ? 1. : -1.);
    float color_mult = 0.01 * (reverse ? -1.0 : 1.0) * color_rotation_speed * (color_rotation_direction ? -1. : 1.);

    float angle  = atan(uv.y - center.y, uv.x - center.x) * 180. / 3.14159265359;
    float distance = length(uv-center);
    float  value    = cos(animation_mult * distance / (0.001 * spacing)  + iTime);

    float v = pow((value+1.) * 0.5, (5. - thickness));
    float h = abs(mod(angle + distance + iTime * color_mult * color_rotation_speed, 360.));
    float s = 1.;

    fragColor = hsv2rgb(h,s,v);
}
