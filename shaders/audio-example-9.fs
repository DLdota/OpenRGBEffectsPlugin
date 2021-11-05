#define c1 vec3(0,0,1)
#define c2 vec3(1,0,1)

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord.xy / iResolution.xy;
    vec2 center = vec2(0.5);
    float v = iAudio[int(abs(center.x - uv.x) * 256.) + int(abs(center.y - uv.y) * 256.)];
    fragColor = v*vec4(mix(c1, c2, 2.*distance(uv,center)), 1.);
}
