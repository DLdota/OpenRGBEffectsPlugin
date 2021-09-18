// https://en.wikipedia.org/wiki/Superformula

#define THICKNESS 0.8
#define FREQ 0.2
#define SCALE 0.8
#define color1 vec4(1.0,0.0,1.0,1.0)
#define background vec4(0.0,0.0,1.0,1.0)


float sf(float m1, float m2, float a, float b, float n1, float n2, float n3, float phi ) {
    return pow(pow(abs(cos(m1*phi/4.)/a), n2) + pow(abs(sin(m2*phi/4.)/b), n3), -.1/n1);
}

vec2 rotateUV(vec2 uv, float rotation, float mid)
{
    return vec2(
      cos(rotation) * (uv.x - mid) + sin(rotation) * (uv.y - mid) + mid,
      cos(rotation) * (uv.y - mid) - sin(rotation) * (uv.x - mid) + mid
    );
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 p = fragCoord.xy / iResolution.xy - 0.5;
    vec2 uv = (10./SCALE) * rotateUV(p, 1.7*iTime, 0.);

    float st = 0.2+abs(sin(1.2*iTime));
    float s = sin(iTime);

    float
          m1 = 15.+ 2.*st,
          m2 = 15. +3.*st,
          a  = 7.+7.*st,
          b  = 7.+ 4.*st,
          n1 = 0.2 + 0.9 * st,
          n2 = 0.2 + 7.* st,
          n3 = 4.+ 4.*st,

          rd = length(uv) - sf(m1, m2, a, b, n1, n2, n3, FREQ * (2.+s) * atan(uv.y/uv.x));

    fragColor = background + vec4(step(THICKNESS*(2.+s), abs(rd))) * color1;

}
