#define c1 vec3(0,0,1)
#define c2 vec3(1,0,1)

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord.xy / iResolution.xy;

    float x = floor(uv.x * 16.);
    float y = floor(uv.y * 16.);

    fragColor = iAudio[int(y*16. + x)]*vec4(mix(c1, c2, uv.y), 1.);
}
