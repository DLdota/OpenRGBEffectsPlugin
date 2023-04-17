#define col vec3(1,0,1)
#define PI 3.14159
#define double_bars true

float dist(vec2 a, vec2 b, vec2 c)
{
   vec2 v1 = b - a;
   vec2 v2 = a - c;
   vec2 v3 = vec2(v1.y,-v1.x);
   return abs(dot(v2,normalize(v3)));
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord/iResolution.xy;

    float amp = 0.;

    for (int i = 0; i < 256; i++) amp+=iAudio[i];

    amp /= 100.;

    vec2 start = vec2(0.5,0.);
    vec2 end = vec2(min(amp,1.), 0.2);

    float val = pow(1.-dist(end, start, double_bars?2.*abs(uv-start):uv), 10.);

    fragColor = vec4(val*HSVToRGB(vec3(mod(100.*iTime+uv.y*180.,360.)/360., 1.,1.)), 1.);
}
