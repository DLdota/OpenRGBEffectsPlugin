#define pi 3.14
#define center_size 0.1
#define inner_circle 0.08
#define color_spread 360.
#define max_size 0.5
#define off_color vec3(1,0,1)
#define background_color vec3(0,1,0)

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
    float a = (pi + atan(uv.x-center.x, uv.y-center.y))/(2.*pi);
    float dist = distance(center, uv);
    float amp = pow(1./dist, 2.);


    if(dist>center_size)
    {
        vec4 c = HSVToRGB(mod(color_spread*dist + 4.*iTime,360.), 1., 1.);
        fragColor = amp*iAudio[int(a*256.)] * c;
    }
    else if(dist<inner_circle)
    {
        fragColor = HSVToRGB(mod(10.*iTime + uv.x*360.,360.), 1., 1.);
    }

    if(dist > max_size)
    {
        fragColor += vec4(2.*(dist-max_size)*off_color,1.);
    }

}
