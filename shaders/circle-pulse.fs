#define speed 1.5
#define stop 1.0
#define color1 vec3(1.0,0.0,1.0)
#define color2 vec3(0.0, 1.0, 1.0)

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = 2.0 * (fragCoord.xy / iResolution.xy) - 1.0;
    fragColor = mix(vec4(color1, 1.0), vec4(color2, 1.0), (uv.x * uv.x + uv.y * uv.y) * stop + sin(iTime * speed));
}
