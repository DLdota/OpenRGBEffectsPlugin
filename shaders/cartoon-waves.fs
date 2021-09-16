// https://www.shadertoy.com/view/ldjBRw
void mainImage(out vec4 O, vec2 U)
{
    O.xyz = iResolution;
    float y, i=-15., k=O.y, c;

        U /= k;

    while (i++ < 15.)
        c = exp(-.1*i*i),
        y = (.08 + .02 * sin(iTime*2.+i*2.)) * exp(-.01*i*i)
            * sin(iTime*2. + U.x / (.2-.1*c) + i*4. )
            - i/2. + 1.5 - U.y,
        O += max(0., 1.-exp(-y*k*c) )
             * (
                 tanh(40.*y)
                 * (0.5 + .4 * sin(iTime+i+vec4(0,1,1,0)))
                 - O
             );
}
