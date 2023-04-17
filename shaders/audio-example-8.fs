#define color_freq 100.
#define color_speed -100.

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord.xy / iResolution.xy;

    float v = iAudio[int(abs(uv.y*256.))] * (1.-uv.y);
    float h = mod(color_speed * iTime + color_freq * uv.y, 360.) / 360.;
    float s = 1.0;

    fragColor = vec4(HSVToRGB(vec3(h, s, v)), 1.);
}
