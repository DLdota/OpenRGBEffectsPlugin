#define center vec2(0.5,0.5)
#define color1 vec4(1.,0.,0.,1.)
#define color2 vec4(0.,0.,1.,1.)
#define size 0.7
#define speed 2.

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord.xy / iResolution.xy;
    uv.x += sin(speed*iTime);
    fragColor = mix(color1, color2, (1./size) * distance(uv, center));
}
