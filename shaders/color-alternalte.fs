#define background vec4(0,0,1,1)
#define animation_speed 10.
#define spread 1.0
#define rotate false

vec2 rotateUV(vec2 uv, float rotation, float mid)
{
    return vec2(
      cos(rotation) * (uv.x - mid) + sin(rotation) * (uv.y - mid) + mid,
      cos(rotation) * (uv.y - mid) - sin(rotation) * (uv.x - mid) + mid
    );
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = (fragCoord.xy / iResolution.xy - 0.5) * 2.* sin(animation_speed * iTime);

    if(rotate)
    {
        uv = rotateUV(uv, iTime, 0.);
    }

    fragColor = background+vec4(pow(spread - uv.x, 3.), 0.,0.,1.);
}
