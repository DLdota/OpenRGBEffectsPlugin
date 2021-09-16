#define WOBBLE false
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    float t = iTime;
    vec2 uv = (fragCoord.xy / iResolution.xy)*2.0-1.0;
    float angle = atan(uv.y,uv.x) + t;
    float k = WOBBLE ? sin(t)*2.0 : 1.0;
    angle -= length(uv) * k;

    vec3 gold = vec3(1.0, 0.94, 0.0);
    fragColor = vec4(gold*vec3(cos(8.*angle)),1.0);
}
