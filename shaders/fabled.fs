// https://www.shadertoy.com/view/7dy3R3
// change the seed to any not-too-huge float and the colors/shapes will change.

#define SEED 420.69
#define SEED_OFFSET 23.4567
#define C1 0.0191725133961645
#define C2 0.0710436067037893
#define C3 0.0497004779019703
#define scale 0.03

vec3 swayRandomized(vec3 seed, vec3 value)
{
    return sin(seed.xyz + value.zxy - cos(seed.zxy + value.yzx) + cos(seed.yzx + value.xyz));
}

vec3 cosmic(vec3 c, vec3 con)
{
    return (con
    + swayRandomized(c, con.yzx)
    + swayRandomized(c + 1.1, con.zxy)
    + swayRandomized(c + 2.2, con.xyz)) * 0.25;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec3 COEFFS = fract((SEED + SEED_OFFSET) * vec3(C1, C2, C3)) + 0.5;

    vec2 uv = fragCoord/iResolution.xy * (1./scale) + swayRandomized(COEFFS.zxy, (iTime * 0.1875) * COEFFS.yzx).xy * 32.0;

    float aTime = iTime * 0.0625;
    vec3 adj = vec3(-1.11, 1.41, 1.61);
    vec3 s = (swayRandomized(vec3(34.0, 76.0, 59.0), aTime + adj)) * 0.25;
    vec3 c = (swayRandomized(vec3(27.0, 67.0, 45.0), aTime - adj)) * 0.25;
    vec3 con = vec3(0.0004375, 0.0005625, 0.0008125) * aTime + c * uv.x + s * uv.y;

    con = cosmic(COEFFS, con);
    con = cosmic(COEFFS + 1.618, con + COEFFS);

    fragColor = vec4(swayRandomized(COEFFS + 3.0, con * (3.14159265)) * 0.5 + 0.5,1.0);
}
