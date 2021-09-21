//f0:center x:
float f0 = 0.5;
//f1:center y:
float f1 = 0.5;
//f2:thickness:
float f2 = 0.5;

float r = 0.2;
float g = 0.0;
float b = 0.4;

 // -iTime for reverse
float time = iTime;

vec2 resolution = iResolution.xy;

float m = min(resolution.x, resolution.y);

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 position = fragColor.xy / m - vec2(0.5 * resolution.x / m, 0.5 * resolution.y / m) * 2. *vec2(f0, f1);
    vec2 point = vec2(cos(time), sin(time));
    float c = 0.0;

    float thickness = 0.01 + f2 * 0.05;
    c += thickness / abs(position.x * point.y - position.y * point.x);
    c += thickness / abs(position.x * point.x + position.y * point.y);
    fragColor = vec4(c * (0.5 + position.y) * r, c * g, c * (0.5 - position.y) *b, 1.0);
}
