#define background vec3(0.2, 0.,0.5)
#define pulse vec3(1., 1.,1.)
#define delay 2.
#define glow 0.05
#define breath_freq 1.

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord/iResolution.xy;  
    vec3 ppulse = pulse;
    ppulse.x *= abs(sin(1.*iTime));
    ppulse.y *= abs(sin(3.*iTime));
    ppulse.z *= abs(sin(7.*iTime));
    fragColor = vec4(mix(abs(sin(breath_freq*iTime))*background, ppulse, glow/(abs(uv.x - mod(iTime, 1. + delay) + 4. * glow))), 1.);
}