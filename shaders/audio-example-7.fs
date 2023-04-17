#define PI 3.14159
#define direction -1.
#define spread 200.

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord.xy / iResolution.xy;

    vec2 center = vec2(0.5);

    float a = mod(direction * 0.1 * iTime + 0.5 * (1. + atan(center.x - uv.x, center.y - uv.y) / PI), 1.);

    float dist = spread*length(center-uv);

    float v = iAudio[int(a*256.)];
    float h = mod(dist + 360.0 * a, 360.)/360.;
    float s = 1.0;

    fragColor = vec4(HSVToRGB(vec3(h, s, v)), 1.);
}
