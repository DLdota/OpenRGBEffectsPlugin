vec2 rotateUV(vec2 uv, float rotation, float mid)
{
    return vec2(
      cos(rotation) * (uv.x - mid) + sin(rotation) * (uv.y - mid) + mid,
      cos(rotation) * (uv.y - mid) - sin(rotation) * (uv.x - mid) + mid
    );
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    float amp = 0.;

    // iAudio size is 256
    for(int i = 0; i < 256; i++)
    {
        amp += iAudio[i];
    }

    vec2 p = fragCoord.xy / iResolution.xy - 0.5;
    vec2 uv = rotateUV(p, iTime, 0.);

    float s = abs(sin(iTime));
    float c = abs(cos(iTime));

    vec3 color = vec3(
       s * iAudio[floor((0.5+uv.x)*128.)],
       c * iAudio[floor((0.5+uv.x+uv.y)*128.)],
       iAudio[floor((0.5+uv.y)*128.)]
    );

    fragColor = abs(vec4(color,1.0));
}
