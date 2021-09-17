// forked from https://www.shadertoy.com/view/ls2SWR

#define freq_x 0.1
#define freq_y 0.1

#define r_mult 1.0
#define g_mult 0.2
#define b_mult 1.0

#define speed_x 1.0
#define speed_y 2.0

void mainImage( out vec4 fragColor, in vec2 fragCoord ){

        vec2 uv = fragCoord.xy / iResolution.xy;
        fragColor =

        vec4(
                r_mult * (sin(fragCoord.y * freq_y)+1.+sin(speed_y*(iTime+180.))),
                g_mult * sin(iTime),
                b_mult * (sin(fragCoord.x * freq_x)+1.+sin(speed_x*iTime)),
                1.0
    );
}
