//https://www.shadertoy.com/view/XdsGzf

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord.xy / iResolution.xy;

    //move the contour up or down based on our time and x-coordinate
    float sweep = sin(iTime * .25) * cos(fragCoord.x * 0.005);
    float px = fragCoord.x + fragCoord.y * (1.75 * sweep + 1.0) + iResolution.x;

    float modAmount = mod(float((px))/64.0, 3.0);

    vec3 tints[3];
    tints[0] = vec3(1.0, .0, .8);
    tints[1] = vec3(.4, .8, .9);
    tints[2] = vec3(.1, .1, 1.0);
    vec3 tint = tints[0];
    if(modAmount > 1.0) tint = tints[1];
    if(modAmount > 2.0) tint = tints[2];
    fragColor = vec4(tint * mod(modAmount, 1.0), 1.0);
}
