//---------------------------------------------------------
// Shader:   ColoredWaves.glsl
// original: http://glslsandbox.com/e#5398.8
// added some mouse interaction
//---------------------------------------------------------

float f1 = 30.0;// scale
float f2 = 15.0; // size
float f3 = 1.5; // gaps
float f4 = 0.5; // position
float f5 = 0.25; //shape modifier y
float f6 = 0.25; //shape modifier x
float f7 = 2.0; // amount

// color channels multipliers
float r = 1.0;
float g = 1.0;
float b = 1.0;

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    float scale = iResolution.y / f1;
    float ring = f2 + floor(- 5.5* 2.0);
    float radius = iResolution.x;
    float gap = scale * f3;
    vec2 pos = fragCoord.xy - iResolution.xy * f4;
    float d = length(pos);

    // Create the wiggle
    d += (sin(pos.y * f5 / scale + iTime) *
      sin(pos.x * f6 / scale + iTime * 0.5)) * scale * f7;

    // Compute the distance to the closest ring
    float v = mod(d + radius / (ring * 2.0), radius / ring);
    v = abs(v - radius / (ring * 2.0) );
    v = clamp(v - gap, 0.0, 1.0);
    d /= radius;
    vec3 m = fract((d - 1.0) * vec3(r * ring * -0.5,g * -ring,b * ring * 0.25) * 0.5);

    fragColor = vec4(m*v, 1.0);
}
