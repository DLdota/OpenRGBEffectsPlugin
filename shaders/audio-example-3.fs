// Make sure to add a AUDIO pass first (iChannelO) and use #version 130 minimum

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord/iResolution.xy;
    vec3 col = 0.5 + 0.5*cos(iTime+uv.xyx+vec3(0,2,4));

    fragColor = vec4(col,1.0) * texture(iChannel0, uv);
}
