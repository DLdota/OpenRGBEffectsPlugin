#define color1 vec3(1.0,0.0,1.0)
#define color2 vec3(0.0,1.0,1.0)
// 1. or -1.
#define direction -1.

vec2 rotateUV(vec2 uv, float rotation, float mid)
{
    return vec2(
      cos(rotation) * (uv.x - mid) + sin(rotation) * (uv.y - mid) + mid,
      cos(rotation) * (uv.y - mid) - sin(rotation) * (uv.x - mid) + mid
    );
}

void mainImage(out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = rotateUV(vec2(fragCoord / iResolution.xy)-vec2(0.5),direction*iTime, 0.);
    fragColor = vec4(uv.x < 0. ? color1:color2, 1.0);
}
