// https://www.shadertoy.com/view/Mds3zM
#define PI 3.14159

float noise(float t)
{
        return fract(sin(t) * 45673.1234);
}

float smoothNoise(float t)
{
        return (noise(t) / 2.0) + (noise(t-1.) / 4.0) + (noise(t+1.) / 4.0);
}

float interpolateNoise(float t)
{
        float fractT = fract(t);
        float intT0  = t - fractT;
        float intT1  = intT0 + 1.0;

        return mix(smoothNoise(intT0), smoothNoise(intT1), fractT);
}

float fbm(float t)
{
        float p;
        p  = 0.500 * interpolateNoise(t); t = t * 2.;
        p += 0.250 * interpolateNoise(t); t = t * 2.;
        p += 0.125 * interpolateNoise(t); t = t * 2.;
        p += 0.062 * interpolateNoise(t);

        p -= 0.2;

        return p;
}


vec4 pointColor(float angle)
{
        float t = mod(angle, PI) / PI;

        float h = t;
        float s = 0.9;
        float v = 0.9;
        return vec4(HSVToRGB(vec3(h, s, v)), 1.);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
        vec2 uv = -1.0 + 2.0 * fragCoord.xy / iResolution.xy;
        uv.x *= iResolution.x / iResolution.y;
        vec2 center = vec2(0.0, 0.0);

        float time = iTime;
        time *= 1.8;

        float R = mod(time, 4.0) * (1.25);

        vec2 point  = uv - center;
        float angle = atan(point.y, point.x);
        float len   = length(point);

        R -= fbm(angle * 20.);

        float d = abs(len - R);
        float t = smoothstep(0.0, 0.5, d);

        fragColor = mix(pointColor(angle), vec4(0.0, 0.0, 0.0, 1.0), t);
}
