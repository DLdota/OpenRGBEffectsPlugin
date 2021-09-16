float pi = atan(1.0)*4.0;
float tau = atan(1.0)*8.0;
float epsilon = 1e-3;
float infinity = 1e6;

//Settings
#define BORDER_COLOR vec3(0.05, 0.20, 1.00)
#define BRIGHTNESS 0.004
#define THICKNESS  0.002

//Checks if a and b are approximately equal.
bool ApproxEqual(float a, float b)
{
    return abs(a - b) <= epsilon;
}

//Distance to a line segment,
float dfLine(vec2 start, vec2 end, vec2 uv, float scale)
{
    start *= scale;
    end *= scale;

    vec2 line = end - start;
    float frac = dot(uv - start,line) / dot(line,line);
    return distance(start + line * clamp(frac, 0.0, 1.0), uv);
}

float dfBorder(vec2 uv, float scale)
{
    float dist = infinity;

    vec2 pt1 = vec2(0.,2.86),
    pt2 = vec2(1.86,2.86),
    pt3 = vec2(2.43,4.63),
    pt4 = vec2(3.,2.86),
    pt5 = vec2(4.86,2.86),
    pt6 = vec2(3.36,1.77),
    pt7 = vec2(3.93,0.),
    pt8 = vec2(2.43,1.10),
    pt9 = vec2(0.93,0.),
    pt10 = vec2(1.51,1.77);

    dist = min(dist, dfLine(pt1, pt2, uv, scale));
    dist = min(dist, dfLine(pt2, pt3, uv, scale));
    dist = min(dist, dfLine(pt3, pt4, uv, scale));
    dist = min(dist, dfLine(pt4, pt5, uv, scale));
    dist = min(dist, dfLine(pt5, pt6, uv, scale));
    dist = min(dist, dfLine(pt6, pt7, uv, scale));
    dist = min(dist, dfLine(pt7, pt8, uv, scale));
    dist = min(dist, dfLine(pt8, pt9, uv, scale));
    dist = min(dist, dfLine(pt9, pt10, uv, scale));
    dist = min(dist, dfLine(pt10, pt1, uv, scale));

    return dist;
}

vec3 DrawStar(vec2 uv, vec3 color, float ind)
{
    float dist = 0.0;
    float shade = 0.0;
    float scale = -ind + mod(iTime-ind,8.)*2. / 6.0;
    if(scale >= 0. /*&& scale < 1.0*/)
    {
        vec2 offs = vec2(5.0, 4.5) * scale/2.0;
        float bright_bord = BRIGHTNESS * min(1.0, 1.0 - sin(0.5 * pi * 50.0) / (0.5 * pi * 1.3));
        //Border
        dist = dfBorder(uv + offs, scale);
        shade = bright_bord / max(epsilon, dist - THICKNESS);
        return color * shade;
    }

    return vec3(0.,0.,0.);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 aspect = iResolution.xy / iResolution.y;
    vec2 uv = fragCoord.xy / iResolution.y - aspect/2.0;

    vec3 color = vec3(0);

    for(float i=0.;i<20.;i++)
    {
        color += DrawStar(uv, vec3(0.86-mod(i,10.)*0.2,0.0,0.48+0.02*mod(i,10.)), 0.1*i);
    }

    fragColor = vec4(color , 1.0);
}
