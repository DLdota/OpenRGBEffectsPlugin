float f0 = 0.1;
float f1 = 1.8;
float f2 = 0.5;
float motion = 0.0;

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    float amp = 4. * f1;
    vec2 pos = (fragCoord.xy / iResolution.xy);
    vec2 position = pos + vec2(f0,f1) / 4.0;
    float bins = 1000.0 * f0 + 3.;    
    float bin = floor((motion*iTime+pos.x) * bins + amp *sin(3.0*iTime)*sin(iTime+pos.y*10.0));
    fragColor = vec4(HSVToRGB(vec3(bin/bins+f2, 1.0, 1.0)), 1.0 );
}
