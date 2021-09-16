//https://www.shadertoy.com/view/Mss3R4

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord.xy / iResolution.xy - vec2(0.5,0.5);
    uv.y *= iResolution.y / iResolution.x;
    float r = sin(iTime*0.1624)*20.0 +  iTime;
    float X = uv.x*-sin(r) + uv.y*cos(r);
    float b = 20.0 * (X + sin(iTime*0.661));

    fragColor = vec4(
            sin(b*0.5)*0.9+0.2,
            0.0,
            -sin(b*0.5)*0.9+0.2,
            1.0);

    fragColor += vec4(
            sin(b-0.5*3.1415),
            sin(b-0.5*3.1415),
            sin(b-0.5*3.1415),
            0.0);
}
