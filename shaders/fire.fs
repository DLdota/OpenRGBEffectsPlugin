#define particles 10.0
#define color_shift 1.5
#define color_mode_speed 2.0
#define density 0.5
#define R 1.
#define G 0.
#define B 0.
#define c 1. // 0. => red, 1. => green, 2. => blue

float rand(vec2 uv) {
    return fract(sin(dot(uv, vec2(12.9898,78.233)))  * 43758.5453123);
}

float perlinNoise(vec2 uv) {
    vec2 ipos = floor(uv);
    vec2 fpos = fract(uv);
    return rand(ipos);
}

float gradient(float y) {
    return smoothstep(0.0, 1.0, y);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord/iResolution.xy * 2.0 + 0.5;

    float noise = perlinNoise(uv * particles +  vec2(0.0, -10.0 * iTime));

    float g = gradient(1.0 - uv.y * 0.4);

    if (noise < density * g) {
        fragColor.r = R;
        fragColor.g = G;
        fragColor.b = B;
        if(c == 0.)
            fragColor.r = 1.-(g * abs(sin(iTime * color_mode_speed) * 0.5 + color_shift) + g * noise);
        else if(c == 1.)
            fragColor.g = 1.-(g * abs(sin(iTime * color_mode_speed) * 0.5 + color_shift) + g * noise);
        else if(c == 2.)
            fragColor.b = 1.-(g * abs(sin(iTime * color_mode_speed) * 0.5 + color_shift) + g * noise);
    }
    else {
        fragColor.rgb = vec3(0.0);
    }

}
