// https://www.shadertoy.com/view/XdXGRM
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord.xy / iResolution.xy - 0.5;
    float r = 3.0+1.0*sin(iTime*0.0);
    vec2 pos = vec2(sin(iTime*0.2)*0.3, sin(iTime*0.5)*0.5);

    uv.x *= sin(uv.y*0.1+iTime);

    float distort = -0.5+0.1*sin(iTime);
    float dist = (distance(pos,uv))*13.14;
    float dx = (10.0+distort*(sin(uv.x*5.0)+dist));
    float dy = 10.0+distort*(sin(uv.y*5.0)+dist)*(uv.y+0.5);

    float d = 5.0*pow(0.125/distance(pos,uv),2.0);

    float j = sin(uv.y*r*dy+iTime*10.0);
    float i = sin(uv.x*r*dx+iTime*10.0);

    float p = clamp(i,0.0,0.82)*clamp(j,0.0,0.8);
    float n = -clamp(i,-0.8,0.0)-0.0*clamp(j,-0.8,0.0);

    float noise = 1.5*(sin(uv.y*1.0)+5.0*(0.5-abs(uv.x)));

    fragColor = (5.0*(vec4(1.0,0.05,0.8,1.0)*n + vec4(0.0,0.0,1.0,1.0)*p)*d);
}
