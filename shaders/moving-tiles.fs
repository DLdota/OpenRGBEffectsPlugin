#define tiles 1.5
#define color1 vec4(1.0,0.0,1.0,1.0)
#define color2 vec4(0.0,0.8,1.0,1.0)

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    float x,y;
        float ratio = iResolution.y / iResolution.x;
        x = fragCoord.x / iResolution.x * tiles;
        y = fragCoord.y / iResolution.y * ratio * tiles;
        float tx = fract(iTime + x);
        float ty = fract(iTime + y);
        fragColor = (tx > 0.5 && ty > 0.5) || (tx < 0.5 && ty < 0.5) ? color1 : color2;
}
