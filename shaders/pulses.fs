//f0:x pos:
float f0 = 0.5;
//f1:y pos:
float f1 = 0.5;
//f2:ring size:
float f2 = 0.8;

float time = iTime;
vec2 resolution = iResolution.xy;

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    float max_distance = 300.0/((resolution.x+resolution.y)/2.0) * 2. * f2;
    float dx = (f0 - fragCoord.x/resolution.x);
    float dy = (f1 - fragCoord.y/resolution.y);
    float pixDist = sqrt(pow(dx,2.0)+pow(dy,2.0));

    float color = 1.0-cos(2.0*3.14)*pixDist*5.0 ;
    float highlight = sin((time-pixDist)*10.0)*(1.0-min(pixDist/max_distance,1.0));

    fragColor = vec4(highlight, color * 0. , highlight + 0.5,1.0);
}
