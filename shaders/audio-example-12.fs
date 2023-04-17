#define pi 3.14
#define center_size 0.1
#define inner_circle 0.08
#define color_spread 360.
#define max_size 0.5
#define off_color vec3(1,0,1)
#define background_color vec3(0,1,0)

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord.xy / iResolution.xy;
    vec2 center = vec2(0.5);
    float a = (pi + atan(uv.x-center.x, uv.y-center.y))/(2.*pi);
    float dist = distance(center, uv);
    float amp = pow(1./dist, 2.);


    if(dist>center_size)
    {
        vec4 c = vec4(HSVToRGB(vec3(mod(color_spread*dist + 4.*iTime,360.)/360., 1., 1.)), 1.);
        fragColor = amp*iAudio[int(a*256.)] * c;
    }
    else if(dist<inner_circle)
    {
        fragColor = vec4(HSVToRGB(vec3(mod(10.*iTime + uv.x*360.,360.)/360., 1., 1.)), 1.);
    }

    if(dist > max_size)
    {
        fragColor += vec4(2.*(dist-max_size)*off_color,1.);
    }

}
