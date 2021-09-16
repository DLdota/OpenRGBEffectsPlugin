// forked from https://www.shadertoy.com/view/4tl3Rn

float roundLookingBlob(vec2 fragCoord, vec2 tPos, float r) {
    vec2 pos = fragCoord.xy/iResolution.yy - vec2(0.5);
    pos.x -= ((iResolution.x-iResolution.y)/iResolution.y)/2.0;
    return pow(max(1.0-length(pos-tPos), 0.0) , r);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    float v = 0.0
        + roundLookingBlob(fragCoord * 0.2,vec2(sin(iTime)* 2.0, cos(iTime)*0.004), 10.0)
        + roundLookingBlob(fragCoord,vec2(sin(iTime*0.6)*0.2, cos(iTime)*0.3), 7.0)
        + roundLookingBlob(fragCoord,vec2(cos(iTime*0.8)*0.3, sin(iTime*1.1)*0.04), 5.0)
        + roundLookingBlob(fragCoord,vec2(cos(iTime*0.2)*0.2, sin(iTime*0.9)*0.05), 8.0)
        + roundLookingBlob(fragCoord,vec2(cos(iTime*1.2)*0.2, 2.0 *sin(iTime*0.9)*0.05), 8.0)
        + roundLookingBlob(fragCoord,vec2(cos(iTime*0.3)*0.4, sin(iTime*1.1)*0.4), 5.0)
        + roundLookingBlob(fragCoord,vec2(sin(iTime*0.6)*0.9, cos(iTime)*0.3), 7.0)
        + roundLookingBlob(fragCoord,vec2(sin(iTime*0.6)*0.3, cos(iTime)*0.8), 7.0)
        + roundLookingBlob(fragCoord,vec2(cos(iTime*0.3)*0.9, sin(iTime*0.1)*0.4), 3.0)
        ;
    v = clamp((v-0.5)*1000.0, 0.0, 1.0);

    float r =
    -1.0 * 1.0 *sin(iTime)
    - 2.0* cos(1.0 * iTime) * fragCoord.x / iResolution.x * fragCoord.y / iResolution.y;
    float g = 0.0 - 0.5 * cos(2.0 * iTime) *  fragCoord.y / iResolution.y;
    float b = 4.0 + sin(iTime) - g + 0.8;
    fragColor = vec4(r * v, v * g, v * b, 0.0);
}

