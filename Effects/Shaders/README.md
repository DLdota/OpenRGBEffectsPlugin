# Shaders effect

This page covers the documentation of the shaders effects.

[[_TOC_]]

## Styles

The editor can have some custom style. If you wish to write a new style, have a look at [the styles folder](https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectsPlugin/-/tree/master/styles). Make your own style and submit a merge request.

## Versions

The shaders effect can support any OpenGL version installed on your system. You have to set the correct version field in the shader editor to match the target version.

| GLSL Version | OpenGL Version | Date              | Shader Preprocessor |
| ---          | ---            | ---               | ---                 |
|1.10.59       | 2.0            | 30 April 2004     | #version 110        |
|1.20.8        | 2.1            | 07 September 2006 | #version 120        |
|1.30.10       | 3.0            | 22 November 2009  | #version 130        |
|1.40.08       | 3.1            | 22 November 2009  | #version 140        |
|1.50.11       | 3.2            | 04 December 200   | #version 150        |
|3.30.6        | 3.3            | 11 March 2010     | #version 330        |
|4.00.9        | 4.0            | 24 July 2010      | #version 400        |
|4.10.6        | 4.1            | 24 July 2010      | #version 410        |
|4.20.11       | 4.2            | 12 December 2011  | #version 420        |
|4.30.8        | 4.3            | 7 February 2013   | #version 430        |
|4.40.9        | 4.4            | 16 June 2014      | #version 440        |
|4.50.7        | 4.5            | 09 May 2017       | #version 450        |
|4.60.5        | 4.6            | 14 June 2018      | #version 460        |

## Shaders

### Concepts

Pixel shaders, also known as fragment shaders, compute color and other attributes of each "fragment": a unit of rendering work affecting at most a single output pixel. 

### The GLSL language

The official OpenGL and OpenGL ES shading language is OpenGL Shading Language, also known as GLSL. GLSL is a high-level shading language with a syntax based on the C programming language.

Syntax overview:

```glsl
int myFunction( int a, int b )
{
    return a+b;
}

void main()
{
    int result = myFunction(1, 2);
}
```

### Built-in functions

The full list can be found here [http://docs.gl/sl4/log](http://docs.gl/sl4/log).

### Shader inputs

```glsl
uniform vec3      iResolution;           // Viewport resolution (in pixels)
uniform float     iTime;                 // Effect time (in seconds, controlled by the speed slider)
uniform sampler2D iChannel0;             // Input channel 0
uniform sampler2D iChannel1;             // Input channel 1
uniform sampler2D iChannel2;             // Input channel 2
uniform sampler2D iChannel3;             // Input channel 3
```

### Passes

3 pass types:

* Audio
* Texture
* Buffer

#### Audio pass

This pass will provide a 2D texture to the next passes. It will use the audio device configured in the effect page.

#### Texture

This pass will provide a 2D texture to the next passes. You can load any valid image files.

#### Buffer

This pass will provide a 2D texture to the next passes. You have to implement the mainImage function.

The main image is a buffer type.

```glsl
// in vec2 fragCoord the coordinates of the current pixel   
// out vec4 fragColor the color of this pixel

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{   
   float red = 1.0;
   float green = 0.0;
   float blue = 0.0;
   float alpha = 1.0;
   
   // let's set the pixel color with our rgb values.
   fragColor = vec4(red, green, blue, alpha);
}
```

## Examples

### Make a static pink color

iChannel0:

```glsl
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
  // blue color
  fragColor = vec4(0.0,0.0,1.0,1.0);
}
```

Main shader:

```glsl
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // pixel coordinates
    vec2 uv = fragCoord.xy / iResolution.xy;

    vec4 red = vec4(1.0,0.0,0.0,1.0);

    // red + blue = pink
    fragColor = red + texture(iChannel0, uv);
}
```

#### A simple color fading (blue - pink - red)


iChannel0:

```glsl
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
  // use sine on blue channel
  fragColor = vec4(0.0,0.0,abs(sin(iTime)),1.0);
}
```

Main shader:

```glsl
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord.xy / iResolution.xy;
    // use cosine on red channel
    vec4 black_to_red = vec4(abs(cos(iTime)),0.0,0.0,1.0);
    fragColor = black_to_red + texture(iChannel0, uv);
}
```

#### A simple static color gradient (red - pink - blue)

iChannel0:

```glsl
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
   vec2 uv = fragCoord.xy / iResolution.xy;
   
  // the more we are on the right, the more it will be blue
  fragColor = vec4(0.0, 0.0, uv.x, 1.0);
}
```

Main shader:

```glsl
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord.xy / iResolution.xy ;
    
    // the more we are on the left, the more it will be red
    vec4 color = vec4(1.-uv.x, 0.0, 0.0, 1.0);

    fragColor = color + texture(iChannel0, uv);
}
```

#### Let's make this gradient move

We will use the iTime input to make it move on x-axis.

iChannel0:

```glsl
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
   vec2 uv = fragCoord.xy / iResolution.xy;
   fragColor = vec4(0.0, 0.0, abs(sin(iTime + uv.x)), 1.0);
}
```

Main shader:

```glsl
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord.xy / iResolution.xy ;
    vec4 color = vec4(abs(sin(iTime - uv.x)), 0.0, 0.0, 1.0);
    fragColor = color + texture(iChannel0, uv);
}
```

#### Draw a circle

```glsl
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord.xy / iResolution.xy;
    vec2 center = vec2(0.5, 0.5);

    float distance = length(uv-center);
    
    if(distance > 0.2 && distance < 0.3)
        fragColor = vec4(1., 0., 0., 1.0);        
    else 
        fragColor = vec4(0.);        
}
```

#### Lets make this circle grow and shrink

```glsl
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // scale a bit
    vec2 uv = 2.*(fragCoord.xy / iResolution.xy - 0.25);
    
    vec2 center = vec2(0.5, 0.5);

    float distance = abs(sin(iTime)) * length(uv - center);
    
    // background blue
    fragColor = vec4(0.,0.,1.,1.);   
    
    // add some red (will be pink)
    if(distance > 0.1 && distance < 0.2)
        fragColor += vec4(1., 0., 0., 1.0);        

}
```

#### Animated bands

```glsl
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord.xy / iResolution.xy;
    
    float bands = 2.;
    
    // setting red channel depending on the (x coordinate + time) modulo number of bands
    // replace uv.x with uv.y to get a vertical scroll
    // change + with - to inverse the direction
    fragColor = vec4(mod(uv.x + iTime, 1./bands), 0., 0., 1.);
}
```

#### Pulses


Same technique as above with some modulo

```glsl
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord.xy / iResolution.xy;
    
    float bands = 2.;
    vec2 center = vec2(0.5);
    float distance = length(uv-center);
    
    // setting red channel depending the x coordinate + time modulo number of bands
    fragColor = vec4(mod(distance - iTime, 1./bands), 0., 0., 1.);
}
```

#### Lets make a function with configurable pulse

```glsl
vec4 circle(vec2 center, float bands, vec2 fragCoord)
{
    vec2 uv = fragCoord.xy / iResolution.xy;    
    float distance = length(uv-center);    
    return vec4(mod(distance - iTime, 1./bands) *2., 0., 0., 1.);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{   
    // create 3 pulses, with different coordinate and bands number.
    fragColor = 
        circle(vec2(0.5), 3., fragCoord) +
        circle(vec2(0.25), 2., fragCoord).yxzw +
        circle(vec2(0.75), 1., fragCoord).zyxw;
}
```

#### Let's add some iTime to the bands number, and mix colors instead of adding them

```glsl
vec4 circle(vec2 center, float bands, vec2 fragCoord)
{
    vec2 uv = fragCoord.xy / iResolution.xy;    
    float distance = length(uv-center);    
    return vec4(mod(distance - iTime, 1./bands) *2., 0., 0., 1.);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{   
    fragColor = 
        mix(
          mix(
            circle(vec2(0.5), 0.5+abs(sin(iTime)) * 0.001, fragCoord) ,
            circle(vec2(0.25), 0.5+abs(sin(10. + iTime))* 0.001, fragCoord).yxzw, 0.5),
            circle(vec2(0.75), 0.5+abs(sin(20. + iTime))* 0.001, fragCoord).zyxw, 0.5);
}
```

#### Make your program configurable

Use define preprocessor to ease the configuration of your program.

```
#define myVar 2.0

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord.xy / iResolution.xy;

    fragColor = vec4(abs(sin(iTime * myVar)),0.,0.,1.);
}
```


## Shadertoy support

We try to support shaders published on Shadertoy.com, there are a few limitations.

No iMouse suppport: even though the iMouse vec4 is in the uniforms, it wont be updated with your current mouse location. The value passed is `vec4(0.,0.,0.,0.)`.

Uniforms not supported (yet):

```
uniform float     iTimeDelta;            // render time (in seconds)
uniform int       iFrame;                // shader playback frame
uniform float     iChannelTime[4];       // channel playback time (in seconds)
uniform vec3      iChannelResolution[4]; // channel resolution (in pixels)
uniform vec4      iDate;                 // (year, month, day, time in seconds)
uniform float     iSampleRate;           // sound sample rate (i.e., 44100)
```


