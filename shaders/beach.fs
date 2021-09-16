// https://www.shadertoy.com/view/XtGSDz

#define horizontal true

float sigmoid(float x) {
    return 1.0 / (1.0 + pow(2.71828, -x));
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 coords = horizontal ? fragCoord.yx : fragCoord.xy;
    vec2 res = horizontal ? iResolution.yx : iResolution.xy;

    vec2 uv = coords.xy / res.xy;
    vec4 water = vec4(0.2, 0.3, 0.8, 1.0);
    vec4 sand = vec4(0.75, 0.6, 0.4, 1.0) * 1.2 + 1.0/14.0;
    sand = mix(sand, sand/4.0, 1.0 - sigmoid(uv.x*10.0 - 6.0));
    water += sin(uv.x) * cos(uv.y) / 2.0;
    water += 1.0 / 10.0;
    water += sin(uv.x) * cos(uv.y);
    uv.x += sin(uv.y * 7.0) / 20.0 * sin(iTime);
    uv.x += sin(uv.y * 3.0) / 10.0 * cos(iTime);
    vec4 wet_sand = mix(sand, vec4(0.8, 0.9, 0.8, 1.0), 1.0 - sigmoid(uv.x*10.0 - 4.0));
    fragColor = mix(water, wet_sand, pow(sigmoid(uv.x*10.0 - 3.0), 2.0));
}
