#define color1 vec3(1.0,0.0,1.0)
#define color2 vec3(0.0,1.0,1.0)
#define color3 vec3(1.0,0.5,0.0)

#define center vec2(0.5,0.5)

const float pi = 3.14159;

vec2 rotateUV(vec2 uv, float rotation, float mid)
{
    return vec2(
      cos(rotation) * (uv.x - mid) + sin(rotation) * (uv.y - mid) + mid,
      cos(rotation) * (uv.y - mid) - sin(rotation) * (uv.x - mid) + mid
    );
}

void mainImage(out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = rotateUV(vec2(fragCoord / iResolution.xy)-center,1.7*iTime, 0.);
    float angle = mod(atan(uv.x, uv.y), 2.*pi);
    fragColor = vec4((angle < 2.*pi/3.)?color1:(angle < 4.*pi/3.)?color2:color3, 1.0);
}
