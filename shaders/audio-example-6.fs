#define PI 3.14159

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
    vec2 center = vec2(0.5,0.5);
    vec2 uvc = uv-center;
    float dist = length(uvc);
    float angle = mod(iTime + atan(uvc.x, uvc.y), 2.*PI)/(2.*PI);
    float h = angle*360.;
    float s = 1.;
    float v = iAudio[int(dist*256.)];
    fragColor = HSVToRGB(h,s,v);
}
