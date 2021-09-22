#define color1 vec3(0,1,1)
#define color2 vec3(1,1,0)
#define color3 vec3(1,0,1)

#define offset1 0.
#define offset2 0.2
#define offset3 0.7

#define speed1 1.
#define speed2 0.5
#define speed3 0.7

vec4 movingBand(vec2 uv, float speed, float offset, vec3 color)
{
    float pos_x = mod(speed*iTime + offset, 1.);
    float dist = uv.x - pos_x;
    float val = dist > 0. && dist < 0.1 ? 1.:0.;
    return vec4(color * val, 1.);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord.xy / iResolution.xy;

    fragColor =
        movingBand(uv, speed1, offset1, color1)+
        movingBand(uv, speed2, offset2, color2)+
        movingBand(uv, speed3, offset3, color3)
        ;
}
