#define c vec2(0.5,0.5)
#define PI 3.14159

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    float dist = length(fragCoord.xy / iResolution.xy-vec2(0.5,0.5));

    float h = mod((iTime + dist), 1.);
    float s = 1.;
    float v = iAudio[int(dist*256.)];

    fragColor = vec4(HSVToRGB(vec3(h,s,v)), 1.);
}
