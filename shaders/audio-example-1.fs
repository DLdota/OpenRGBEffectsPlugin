// set the color
#define c vec3(0,1,1)

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord.xy / iResolution.xy;

    fragColor = vec4(c * iAudio[int(abs(uv.y*256.))] * (1.-uv.y), 1.);
}
