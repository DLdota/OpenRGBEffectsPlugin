// forked from https://www.shadertoy.com/view/MdsXzl

#define ITERATIONS 20.0
#define CIRCLES 20.0
#define R 0.9
#define G 0.8
#define B 0.1

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord.xy / iResolution.xy;
    vec2 center = vec2(0.5, 0.5);
    float d = distance(uv, center) * CIRCLES;
    float adj = cos(iTime) * ITERATIONS;
    float i = 0.5 + sin(d + iTime + adj) / 2.0;
    float j = 0.5 + cos(d + iTime + adj) / 2.0;
    fragColor = vec4(R*j, G*(i + j), B*(i + j), 1.0);
}

