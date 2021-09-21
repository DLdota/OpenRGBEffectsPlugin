//f0:freq:
float f0 = 0.5;
//f1:amp:
float f1 = 0.5;
//f2:rays:
float f2 = 0.5;

float time = iTime;
vec2 resolution = iResolution.xy;

void mainImage( out vec4 fragColor, in vec2 fragCoord ) {

    vec2 pos = ( gl_FragCoord.xy / resolution.xy ) * 2. ;

    float color = 0.0;

    if ( pos.y > f1  +  sin(pos.x * 8. * f0 +time)/2. )
    {
        color = .9;
    }

    if ( pos.x >  sin(time) * 5.) {
        //color *= .92;
    }

    fragColor = vec4( vec3(
        color / sin(cos(time) + cos( pos.y * pos.x / f2)),
        .01+sin( pos.y + pos.x *f2) *  color,
        2. *  color * sin( f2 *pos.y * pos.x)),
    1.0 );

}
