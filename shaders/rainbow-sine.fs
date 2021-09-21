// forked from https://www.shadertoy.com/view/MdsSDj

#define thickness 1.0
#define frequency 4.0
#define speed 4.0
#define amplitude 2.0

vec3 background = vec3(0.0, 0.0, 0.0);

float PI = 3.14159265359;

float dist(float x, vec2 uv)
{
    return abs(uv.y - sin(x)) / sqrt(1.0 + cos(x) * cos(x));
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord.xy / iResolution.xy;
    uv.y -= 0.5;
    uv.y *= (1./amplitude) * iResolution.y / iResolution.x;
    uv *= PI * frequency;
    float t = iTime * speed;

    uv.x -= t;

    float d = dist(uv.x, uv);

    if (d < thickness) {
        float o = (thickness - d) / thickness;
        background.x = o * (sin(uv.x) / 2.0 + 0.5);
        background.y = o * (sin(t) / 2.0 + 0.5);
        background.z = o * (0.5 - sin(uv.x) / 2.0);
    }

    fragColor = vec4(background, 1.0);
}
