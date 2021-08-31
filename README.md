# Effects Plugin

[![pipeline status](https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectsPlugin/badges/master/pipeline.svg)](https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectsPlugin/-/commits/master)

## What is this?

This is a plugin for [OpenRGB](https://gitlab.com/CalcProgrammer1/OpenRGB) that provides various effects that can be synced across devices.

## Downloads

* [Windows 32](https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectsPlugin/-/jobs/artifacts/master/download?job=Windows%2032)
* [Windows 64](https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectsPlugin/-/jobs/artifacts/master/download?job=Windows%2064)
* [Linux 32](https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectsPlugin/-/jobs/artifacts/master/download?job=Linux%2032)
* [Linux 64](https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectsPlugin/-/jobs/artifacts/master/download?job=Linux%2064)
* [MacOS ARM64](https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectsPlugin/-/jobs/artifacts/master/download?job=MacOS%20ARM64)
* [MacOS Intel](https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectsPlugin/-/jobs/artifacts/master/download?job=MacOS%20Intel)

## How do I install it?

* Download and extract the correct files depending on your system
* Launch OpenRGB
* From the Settings -> Plugins menu, click the "Install plugin" button

### Linux

You need `libopenal` installed.


## What Effects do you have?

```
OpenRGBEffectPlugin/Effects
├── Ambient
├── AudioSine
├── AudioStar
├── AudioSync
├── AudioVisualizer
├── Bloom
├── BouncingBall
├── Breathing
├── Bubbles
├── ColorWheelEffect
├── Comet
├── CrossingBeams
├── CustomMarquee
├── DoubleRotatingRainbow
├── Fill
├── FractalMotion
├── GifPlayer
├── GradientWave
├── Hypnotoad
├── Layers
├── Lightning
├── Marquee
├── Mask
├── Mosaic
├── MotionPoint
├── MotionPoints
├── NoiseMap
├── RadialRainbow
├── Rain
├── RainbowWave
├── RotatingBeam
├── RotatingRainbow
├── Sequence
├── SmoothBlink
├── SpectrumCycling
├── Spiral
├── Stack
├── StarryNight
├── Sunrise
├── Swap
├── SwirlCircles
├── SwirlCirclesAudio
├── Visor
└── Wavy
```

## Common Issues

### OpenRGB does not start with the plugin

OpenRGB and EffectsPLugin are under heavy development, make sure to run latest pipeline from both.

### My CPU usage is really high

There are a few effects that cause this:

* Audio effects
* Ambient

Both of the audio effects will spike in CPU usage when opening an [S/PDIF device](https://en.wikipedia.org/wiki/S/PDIF) for reasons we still don't know. This can be fixed by switching off of the spdif device or stopping the effect.

The very nature of ambient is to do massive calulation to get the average or most common color on screen. Scaling uses a lot of CPU so once you have selected your portion of the screen it is recommended that you hide the preview.

## Contributing

Please read the [contributing guide](./CONTRIBUTING.md) if you want to add effects or bring new features.

