#define PI 3.14159
#define direction -1.
#define spread 200.

vec4 HSVToRGB(float h, float s, float v)
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

    vec2 center = vec2(0.5);

    float a = mod(direction * 0.1 * iTime + 0.5 * (1. + atan(center.x - uv.x, center.y - uv.y) / PI), 1.);

    float dist = spread*length(center-uv);

    float v = iAudio[int(a*256.)];
    float h = mod(dist + 360.0 * a, 360.);
    float s = 1.0;

    fragColor = HSVToRGB(h, s, v);
}