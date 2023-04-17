#define c vec2(0.5,0.5)
#define gain 100.
#define PI 3.14159

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 p = abs(fragCoord.xy / iResolution.xy - 0.5);
    float v = clamp(gain*(0.5-p.y) * iAudio[int(floor((p.x)*512.))], 0.0, 1.0);

    float h = mod(p.x+iTime, PI) / PI ;
    float s = 1.0;

    fragColor = vec4(HSVToRGB(vec3(h, s, v)), 1.);
}

