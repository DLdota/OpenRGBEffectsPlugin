// by wststreet // www.kamehamehaaa.org // wststreet2@gmail.com


//f0:wave 1:
float f0 = 0.25;
//f1:wave 2:
float f1 = 0.25;
//f2:highlights:
float f2 = 0.95;

float band_size = 0.00005;

vec3 color1 = vec3(0.8,0.3,0.5);
vec3 color2 = vec3(0.4,0.7,1.0);
vec3 color3 = vec3(0.0,0.0,0.0);

float time = iTime;
vec2 resolution = iResolution.xy;

float line(float x){
    return -0.01*sin(30.0*x+time) / f0+
    0.02/f1*sin(10.0*x+time*1.0)+
    -0.03*sin(3.0*x+time*0.5)+0.5;
}

float hdist(float x, float y){
    return (y-x)>0.0?y-x*f2:x-y*f2;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{

    vec2 position = ( gl_FragCoord.xy / resolution.xy );

    vec4 color ;

    float lineposx = line(position.x);

    if(position.y < lineposx-band_size)
    {
        color=vec4(color1 ,1.0);
    }
    else if (position.y > lineposx+band_size)
    {
        color=vec4(color2,1.0);
    }
    else
    {
        color=vec4(color3,1.0)/abs(200.0*(position.y-lineposx));
    }

    float t_color = hdist(position.y,line(position.x));

    color -= vec4(t_color, t_color, t_color, t_color);

    fragColor = color;
}
