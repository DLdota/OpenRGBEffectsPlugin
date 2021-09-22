#define beam_color vec4(1.,0.,1.,1.)
#define dir -1.

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord.xy / iResolution.xy;

    float y = mod(dir * iTime, 1.5);
    float str = -pow((uv.y - y) * 110., 2.) + .8;
    uv.x -= clamp(str * .01, 0., 1.);
    float beam = pow(1. - pow(abs(uv.y - y), .3), 3.);

    fragColor = beam * beam_color;
}
