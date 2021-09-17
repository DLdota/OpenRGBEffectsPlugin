#define PI 3.14159
#define TWO_PI (PI*2.0)
#define FREQ 2.0
#define time -iTime

// shape
#define x00 1.0
#define x01 1.0
#define x10 1.0
#define x11 1.0

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = (fragCoord.xy / iResolution.xy -0.5) * FREQ;

    float a = atan(uv.y,uv.x);
    mat2 hyper = mat2(-cos(x00*a), sin(x01*a), sin(x10*a), cos(x11*a));
    uv = abs(mod(uv*hyper+time,vec2(2.0))-1.0);

    vec3 color = vec3(0.5+0.5* cos(uv.y+time*0.5), 2.*uv.x, 0.5+0.5*sin(uv.y+time*0.3));
    fragColor = abs(vec4(color,1.0));
}
