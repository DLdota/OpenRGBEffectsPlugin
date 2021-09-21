//---------------------------------------------------------------------------------------------------
// https://github.com/hughsk/glsl-hsv2rgb/blob/master/index.glsl
// http://glslsandbox.com/e#49720.0

//f0:count:
float f0 = 0.1;
//f1:amp speed:
float f1 = 1.8;
//f2:offset:
float f2 = 0.5;

float motion = 0.0;

float time = iTime;
vec2 resolution = iResolution.xy;

vec3 hsv2rgb(vec3 c) {
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord ) {
    float amp = 4. * f1;
    vec2 position = ( fragCoord.xy / resolution.xy ) + vec2(f0,f1) / 4.0;
    float bins = 1000.0 * f0 + 3.;
    vec2 pos = (fragCoord.xy / resolution.xy);
    float bin = floor((motion*time+pos.x) * bins + amp *sin(3.0*time)*sin(time+pos.y*10.0));
    fragColor = vec4(hsv2rgb(vec3(bin/bins+f2, 1.0, 1.0)), 1.0 );
}
