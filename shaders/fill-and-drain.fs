// Define colors
#define c1 vec3(0,0,0)
#define c2 vec3(1,0,1)

// direction, left to right : 1. or -1.
#define direction 1.

// horizontal or vertical
#define horizontal true

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord.xy / iResolution.xy;
    float t = direction*iTime;
    bool step = mod(t, 2.) >= 1.;
    float coord = horizontal ? uv.x : uv.y;
    fragColor = coord < mod(t, 1.) ? vec4(step?c1:c2, 1.): vec4(step?c2:c1, 1.);
}
