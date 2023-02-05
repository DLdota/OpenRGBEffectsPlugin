# Effects Plugin

[![pipeline status](https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectsPlugin/badges/master/pipeline.svg)](https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectsPlugin/-/commits/master)

## What is this?

This is a plugin for [OpenRGB](https://gitlab.com/CalcProgrammer1/OpenRGB) that provides various effects that can be synced across devices.

## Experimental (Master)

* [Windows 64](https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectsPlugin/-/jobs/artifacts/master/download?job=Windows%2064)
* [Buster 64](https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectsPlugin/-/jobs/artifacts/master/download?job=Buster%2064)
* [Bullseye 64](https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectsPlugin/-/jobs/artifacts/master/download?job=Bullseye%2064)
* [Bookworm 64](https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectsPlugin/-/jobs/artifacts/master/download?job=Bookworm%2064)
* [MacOS ARM64](https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectsPlugin/-/jobs/artifacts/master/download?job=MacOS%20ARM64)
* [MacOS Intel](https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectsPlugin/-/jobs/artifacts/master/download?job=MacOS%20Intel)

## Stable (0.8)

* [Windows 64](https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectsPlugin/-/jobs/3418186147/artifacts/download)
* [Linux 64](https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectsPlugin/-/jobs/3418186145/artifacts/download)
* [MacOS ARM64](https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectsPlugin/-/jobs/3418186148/artifacts/download)
* [MacOS Intel](https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectsPlugin/-/jobs/3418186149/artifacts/download)

## How do I install it?

* Download and extract the correct files depending on your system
* Launch OpenRGB
* From the Settings -> Plugins menu, click the "Install plugin" button

### Linux

You need `libopenal` installed.

#### Fedora

You need `openal-soft` installed.
```

sudo dnf install openal-soft

```

### OSX

Install openal-soft

```
brew install openal-soft
```


## What Effects do you have?

```
├── Ambient
├── AudioParty
├── AudioSine
├── AudioStar
├── AudioSync
├── AudioVisualizer
├── Bloom
├── BouncingBall
├── Breathing
├── BreathingCircle
├── Bubbles
├── ColorWheelEffect
├── Comet
├── CrossingBeams
├── CustomGradientWave
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
├── MovingPanes
├── NoiseMap
├── RadialRainbow
├── Rain
├── RainbowWave
├── RotatingBeam
├── RotatingRainbow
├── Sequence
├── Shaders
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

