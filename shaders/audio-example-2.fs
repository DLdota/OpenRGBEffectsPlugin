void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord.xy / iResolution.xy;

    float s = abs(sin(iTime));
    float c = abs(cos(iTime));

    vec4 color_x = vec4(
       s * iAudio[int(floor((uv.x)*256.))],
       c * iAudio[int(floor((uv.y)*256.))],
       s * iAudio[int(floor((uv.y)*256.))],
       1.
    );

    vec4 color_y = vec4(
       s * iAudio[int(floor((uv.y)*256.))],
       c * iAudio[int(floor((uv.x)*256.))],
        c * iAudio[int(floor((uv.x)*256.))],
       1.
    );

    fragColor = abs(color_x + color_y);
}
