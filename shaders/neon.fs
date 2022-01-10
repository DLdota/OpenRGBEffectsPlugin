// forked from https://www.shadertoy.com/view/7lVXDy
#define PI 3.141592

float stepping(float t)
{
    if(t < 0.)
    {
        return -1. + pow(1. + t, 2.);
    }
    else
    {
        return 1. - pow(1. - t , 2.);
    }
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = (fragCoord * 2. - iResolution.xy) / iResolution.y;
    fragColor = vec4(0);
    uv = normalize(uv) * length(uv);

    for(int i=0;i<12;i++)
    {
        float t = iTime + float(i) * PI / 12. * (5. + 1. * stepping(sin(iTime * 3.)));
        vec2 p = vec2(cos(t), sin(t));
        p *= cos(iTime + float(i) * PI * cos(iTime / 8.));
        vec3 col = cos(vec3(0, 1, -1) * PI * 2. / 3. + PI * (iTime / 2. + float(i) / 5.)) * 0.5 + 0.5;
        fragColor += vec4(0.05 / length(uv - p * 0.9) * col, 1.0);
    }

    fragColor.xyz = pow(fragColor.xyz, vec3(3.));
    fragColor.w = 1.0;
}
