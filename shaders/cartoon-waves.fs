// https://www.shadertoy.com/view/ldjBRw

float fast_tanh(float x){
  float x2 = x * x;
  float a = x * (135135. + x2 * (17325. + x2 * (378. + x2)));
  float b = 135135. + x2 * (62370. + x2 * (3150. + x2 * 28.));
  return a / b;
}

void mainImage(out vec4 O, in vec2 U)
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
                 fast_tanh(40.*y)
                 * (0.5 + .4 * sin(iTime+i+vec4(0,1,1,0)))
                 - O
             );
}
