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

float dist(vec2 a, vec2 b, vec2 c)
{
   vec2 v1 = b - a;
   vec2 v2 = a - c;
   vec2 v3 = vec2(v1.y,-v1.x);
   return abs(dot(v2,normalize(v3)));
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord/iResolution.xy;

    float amp = 0.;

    for (int i = 0; i < 256; i++) amp+=iAudio[i];

    amp /= 100.;

    vec2 start = vec2(0.5,0.);
    vec2 end = vec2(min(amp,1.), 0.2);

    float val = pow(1.-dist(end, start, uv), 10.);

    fragColor = val*HSVToRGB(mod(100.*iTime+uv.y*180.,360.), 1.,1.);
}
