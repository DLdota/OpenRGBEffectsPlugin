//forked from https://www.shadertoy.com/view/4tlGRX

#define freq 16.0
#define fov 0.5
#define background vec3(0.,0.,0.)

mat2 mm2(in float a){float c = cos(a), s = sin(a);return mat2(c,-s,s,c);}

vec3 tex(in vec2 p)
{
    return vec3(1.)*smoothstep(0.1, 1.05, max(sin((p.x)*freq),sin((p.x)*freq)));
}

vec3 cubeproj(in vec3 p)
{
    vec3 x = tex(p.zy/p.x);
    vec3 y = tex(p.xz/p.y);
    vec3 z = tex(p.xy/p.z);
    x *= vec3(1,0,0)*abs(p.x) + p.x*vec3(0,1,0);
    y *= vec3(0,1,0)*abs(p.y) + p.y*vec3(0,0,1);
    z *= vec3(0,0,1)*abs(p.z) + p.z*vec3(1,0,0);
    p = abs(p);
    if (p.x > p.y && p.x > p.z) return x;
    else if (p.y > p.x && p.y > p.z) return y;
    else return z;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    
    float amp = 0.;
    
    int min_freq = 0;
    int max_freq = 3; //lowered max freq to make the effects pop more
    for(int i = min_freq; i < max_freq; i++)
    {
        amp += iAudio[i];
    }
    
    amp = min(1., amp);
    
    
    vec2 p = fragCoord.xy / iResolution.xy;
    vec2 p2 = fragCoord.xy/iResolution.xy-0.5;

    p2.x*=iResolution.x/iResolution.y;
    //p2*= fov;
    p2*= pow(fov,amp);
    
    vec3 ro = vec3(0.);
    vec3 rd = normalize(vec3(p2,-0.5));
    mat2 mx = mm2(iTime / 2.0);
    mat2 my = mm2(iTime / 2.0);
    ro.xz *= mx;rd.xz *= mx;
    ro.xy *= my;rd.xy *= my;

    vec3 col = cubeproj(rd)*1.1;

    col.x *= sin(iTime + p.x+ p.y);
    col.y *= sin(1.23*iTime + p.x + p.y);
    col.z *= sin(1.57*iTime + p.x+ p.y);

    fragColor = vec4(background + abs(col), 1.0 );
}
