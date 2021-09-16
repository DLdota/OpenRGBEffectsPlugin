/*
Shader inputs:

uniform vec3      iResolution;           // viewport resolution (in pixels)
uniform float     iTime;                 // shader playback time (in seconds)

You must implement:

    "void mainImage( out vec4 fragColor, in vec2 fragCoord )"

Exemple:
*/

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    float r = abs(sin(2.0 * iTime));
    float g = abs(sin(3.0 * iTime));
    float b = abs(sin(7.0 * iTime));

    fragColor = vec4(r, g, b, 1.0);
}
