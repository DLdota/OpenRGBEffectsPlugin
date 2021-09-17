#define color1 vec3(1.0,0.0,1.0)
#define color2 vec3(0.0,1.0,1.0)
#define divs 2.0
const float pi = 3.14159;

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 div = vec2( divs, divs * iResolution.y / iResolution.x );
    vec2 xy = div*(fragCoord.xy / iResolution.xy - 0.5);
    vec2 sxy = sin(pi*vec2((xy.x + xy.y)*(xy.x - xy.y)*0.5 - iTime*1.0, xy.x*xy.y));
    vec2 b = fwidth(sxy)*0.7071;
    vec2 pxy = smoothstep( -b, b, sxy );
    pxy = 2.0*pxy - 1.0;
    float c1 = sqrt( 0.5 * (pxy.x * pxy.y) + 0.5 );
    float c2 = 1.0 - c1;
    fragColor = vec4( c1*color1 + c2 * color2, 1.0);
}
