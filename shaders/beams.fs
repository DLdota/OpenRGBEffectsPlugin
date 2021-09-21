#define PI 3.1415926535897932384626

float time = iTime;

vec2 resolution = iResolution.xy;

//f0:start zoom:
float f0 = 0.5;
//f1:vertical spread:
float f1 = 0.05;
//f2:horizontal spread:
float f2 = 0.05;

vec3 color = vec3(0.0, 0.3, 0.5);

void mainImage( out vec4 fragColor, in vec2 fragCoord ) {

    vec2 p = (gl_FragCoord.xy * 2.0 - resolution) / min(resolution.x, resolution.y) * f0;

    float f = 0.0;

    for(float i = 0.0; i < 100.0; i++)
    {
        float s = sin(time + i * PI / 4.0 ) * 0.8 +f1;
        float c = cos(time + i * PI / 5.0 ) * 0.8 +f2;

        f += 0.001 / (abs(p.x + c) * abs(p.y + s));
    }

    fragColor = vec4(vec3(f * f * color), 1.0);
}
