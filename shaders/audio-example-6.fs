#define PI 3.14159

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord.xy / iResolution.xy;
    vec2 center = vec2(0.5,0.5);
    vec2 uvc = uv-center;
    float dist = length(uvc);
    float angle = mod(iTime + atan(uvc.x, uvc.y), 2.*PI)/(2.*PI);
    float h = angle;
    float s = 1.;
    float v = iAudio[int(dist*256.)];
    fragColor = vec4(HSVToRGB(vec3(h,s,v)), 1.);
}
