/*
Make your own shader program.

Make sure to read the documentation.

https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectsPlugin/-/blob/master/Effects/Shaders/README.md
*/

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    float r = abs(sin(2.0 * iTime));
    float g = abs(sin(3.0 * iTime));
    float b = abs(sin(7.0 * iTime));

    fragColor = vec4(r, g, b, 1.0);
}
