#define c1 vec3(0,0,1)
#define c2 vec3(1,0,1)

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord.xy / iResolution.xy;
    vec2 center = vec2(0.5);
    float value = iAudio[int(abs(uv.x-center.x)*256.)] * iAudio[int(abs(uv.y-center.y)*256.)];
    fragColor = value * vec4(mix(c1, c2, 2.*distance(uv,center)), 1.);
}
