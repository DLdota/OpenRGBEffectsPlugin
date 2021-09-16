// https://www.shadertoy.com/view/Mdf3W8

float bump(float x) {
        return abs(x) > 1.0 ? 1.0*x : x ;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = (fragCoord.xy / iResolution.xy);

    float timeScroll = iTime* 1.0;
    float sinusCurve = sin((uv.x*2.0+timeScroll)/0.5)*0.3;

    uv.y = (uv.y * 2.0 - 1.00) + sinusCurve;

    float line = abs(0.1 /uv.y);

    vec3 color = vec3(0.0);

    color.x = bump( sin(iTime)*(uv.x - 1.0));
    color.y = bump( sin(iTime)*(uv.x - 0.0));
    color.z = bump( sin(iTime)*(uv.x - 0.0));

    fragColor = vec4(color, 0.0);
}
