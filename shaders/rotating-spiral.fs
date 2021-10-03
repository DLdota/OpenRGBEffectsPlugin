#define c1 vec3(1.0, 0.0, 1.0)
#define c2 vec3(0.0, 1.0, 1.0)
#define divs 1.
#define speed_mult 2.
#define dir -1.

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = (fragCoord.xy / iResolution.xy)*2.0-1.0;
    float angle = atan(uv.y,uv.x) + dir * iTime * speed_mult;
    angle -= length(uv);

    float c = cos(divs*angle);

    if(c>0.)
    {
        fragColor = vec4(c1*vec3(c),1.0);
    }
    else
    {
        fragColor = vec4(c2*vec3(cos(divs*angle + 3.14)),1.0);
    }
}
