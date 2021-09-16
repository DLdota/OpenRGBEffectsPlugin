//https://www.shadertoy.com/view/4ds3WH

#define halfPhase 5.5
#define speed_modifier 1.5
#define scale 5.0

void mainImage( out vec4 fragColor, in vec2 fragCoord ) {

        float activeTime = iTime * speed_modifier;
        vec3 col;
        float timeMorph = 10.0;

        vec2 p = -1.0 + 2.0 * fragCoord.xy / iResolution.xy;

        p *= scale;

        float a = atan(p.y,p.x);
        float r = sqrt(dot(p,p));

        if(mod(activeTime, 2.0 * halfPhase) < halfPhase)
                timeMorph = mod(activeTime, halfPhase);
        else
                timeMorph = (halfPhase - mod(activeTime, halfPhase));

        timeMorph = 2.0*timeMorph + 1.0;

        float w = 0.25 + 1.0*(sin(activeTime + 1.0*r)+ 3.0*cos(activeTime + 2.0*a)/timeMorph);
        float x = 0.8 + 3.0*(sin(activeTime + 1.0*r)+ 3.0*cos(activeTime + 2.0*a)/timeMorph);

        col = vec3(0.1*w*x,0.0,0.82*w*x)*1.1;

        fragColor = vec4(col,1.0);

}
