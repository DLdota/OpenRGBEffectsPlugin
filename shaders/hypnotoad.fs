#define reverse true
#define animation_speed 1.
#define animation_direction false
#define color_rotation_speed 100.
#define color_rotation_direction false
#define center vec2(0.5)
#define spacing 1.
#define thickness 1.

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord.xy / iResolution.xy;

    float animation_mult = 0.01 * (reverse ? -1.0 : 1.0) * animation_speed * (animation_direction ? 1. : -1.);
    float color_mult = 0.01 * (reverse ? -1.0 : 1.0) * color_rotation_speed * (color_rotation_direction ? -1. : 1.);

    float angle  = atan(uv.y - center.y, uv.x - center.x) * 180. / 3.14159265359;
    float distance = length(uv-center);
    float  value    = cos(animation_mult * distance / (0.001 * spacing)  + iTime);

    float v = pow((value+1.) * 0.5, (5. - thickness));
    float h = abs(mod(angle + distance + iTime * color_mult * color_rotation_speed, 360.))/360.;
    float s = 1.;

    fragColor = vec4(HSVToRGB(vec3(h,s,v)), 1.);
}
