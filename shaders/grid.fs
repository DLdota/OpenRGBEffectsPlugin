const float RADIUS = 20.;
const float SOFTNESS = 0.00085;
const float SCALE = 1.5;
const float SPEED_MULT = 1.5;

float rand(vec2 n)
{
    return fract(sin(n.x*2732.7357+n.y*2542.3643)*4365.6247);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    float time = iTime*SPEED_MULT;
    vec2 pos = (fragCoord.xy / iResolution.xy) * 2. - 1.;
    pos.x *= iResolution.x / iResolution.y;

    vec2 spot = floor(pos *SCALE)/SCALE;
    float base = rand(spot);

    float color = mod(base * time * .5, 2.);
    if(color > 1.) color = 2. - color;
    color = .75 * color + .25;

    float timePerc = mod(time * .1, 2.);
    if(timePerc > 1.) timePerc = 2. - timePerc;

    vec3 lefta = vec3(1., .5, 1.);
    vec3 leftb = vec3(0., .5, 1.);
    vec3 left = mix(lefta, leftb, timePerc);

    float timePerc2 = mod(time * .08, 2.);
    if(timePerc2 > 1.) timePerc2 = 2. - timePerc2;

    vec3 righta = vec3(0., 0., 1.);
    vec3 rightb = vec3(1., 1., 0.);
    vec3 right = mix(righta, rightb, timePerc2);

    vec3 squareColor = color * mix(left, right, pos.x);

    float len = length(pos);
    float vignette = smoothstep(RADIUS, RADIUS-SOFTNESS, len);

    fragColor = vec4(mix(squareColor, squareColor * vignette, .5), 1.);
}

