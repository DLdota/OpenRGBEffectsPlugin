#define base_color vec3(1.0,0.0,1.0)

#define x_mult 1.5
#define x_mod 90.0
#define y_mod 0.0
#define width 10.
#define color_change_speed 2.

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = (fragCoord.xy / iResolution.xy);
    uv *= sin(x_mult*uv.x);
    vec2 p = vec2(uv.x+x_mod, uv.y+y_mod);
    vec4 color = vec4(base_color, 1.0);

    color += color_change_speed * iTime;

    color = sin(color);

    float l = mod(length(p) + iTime / 2.0,0.3) * width;
    l = floor(l) / 5.0;
    color.r = l;

    fragColor = color;
}
