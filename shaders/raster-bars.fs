// forked from https://www.shadertoy.com/view/lssXWX

#define color1 vec3(1., .5, 0.)
#define color2 vec3(0., .5, 1.)
#define color3 vec3(0.5, 0., 1.)
#define amp1 0.3
#define amp2 0.3
#define amp3 0.3
#define offset1 0.5
#define offset2 0.5
#define offset3 0.5
#define freq1 1.0
#define freq2 1.0
#define freq3 1.0
#define thickness1 0.08
#define thickness2 0.08
#define thickness3 0.08
#define background vec3(0., 0., 0.)

vec3 nrand3( vec2 co )
{
        vec3 a = fract( cos( co.x*8.3e-3 + co.y )*vec3(1.3e5, 4.7e5, 2.9e5) );
        vec3 b = fract( sin( co.x*0.3e-3 + co.y )*vec3(8.1e5, 1.0e5, 0.1e5) );
        vec3 c = mix(a, b, 0.5);
        return c;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 ruv = fragCoord.xy / iResolution.xy;

    vec3 col = background;

    float rps0 = offset1+sin(freq1*iTime)*amp1;
    float rps1 = offset2+cos(freq2*iTime)*amp2;
    float rps2 = offset3+sin(freq3*(3.14159+iTime))*amp3;

    rps0 = abs(ruv.y-rps0);
    rps1 = abs(ruv.y-rps1);
    rps2 = abs(ruv.y-rps2);

    if(rps0 < thickness1) col = color1*pow(1.-(rps0/thickness1), .5);
    if(rps1 < thickness2) col = color2*pow(1.-(rps1/thickness2), .5);
    if(rps2 < thickness3) col = color3*pow(1.-(rps2/thickness3), 0.5);

    fragColor = vec4(col,1.0);
}
