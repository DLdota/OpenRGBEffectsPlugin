// variant of https://www.shadertoy.com/view/XtjfDy

float f1 = 80.0; // ring size
float f2 = 100.0; // spacing
float f3 = 1.0; // speed mult

void mainImage(out vec4 O, vec2 u)
{
    vec2 U = u+u - iResolution.xy;
    float T = 6.2832, l = length(U) / f1, L = ceil(l) * 2.,
          a = atan(U.x,U.y) - iTime * 2.* f3 * (fract(1e2*sin(L))-0.5);
    O = .6 + .4* cos( floor(fract(a/T)*L) + vec4(0,23,21,0) )
        - max(0., f2* max( cos(T*l), cos(a*L) ) - 8. );

}
