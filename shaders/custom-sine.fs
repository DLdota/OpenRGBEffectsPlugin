#define FREQ_MOD 4.0
#define TIME_MOD 2.0
#define AMP_SCALE 0.5
#define THICKNESS 0.2
#define wave vec3(1.0,0.0,1.0)
#define background vec3(0.0,0.0,1.0)

const float PI = 3.14159265359;
const float DBL_PI = PI * 2.0;

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord.xy / iResolution.xy;
    uv.y = -((1.0 - uv.y) - 0.5);
    float ampMod = ((sin(FREQ_MOD*iTime) + 0.25) + 1.0) / 2.0;
    vec2 ptOnWave = vec2(uv.x, sin((uv.x * DBL_PI) + FREQ_MOD * iTime * TIME_MOD) * (AMP_SCALE * ampMod));
    float distToPt = length(uv - ptOnWave);
    float c1 = floor(((1.0 + (THICKNESS)) - distToPt));
    float c2 = 1.0 - c1;
    fragColor = vec4( c1*wave + c2 * background, 1.0);
}
