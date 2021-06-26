# Effects

[![pipeline status](https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectsPlugin/badges/master/pipeline.svg)](https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectsPlugin/-/commits/master)

## What is this?

This is a plugin for [OpenRGB](https://gitlab.com/CalcProgrammer1/OpenRGB) that provides various Effects that can be synced across devices

## How do I install it?

### **windows**

1. download the latest [Windows pipeline](https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectsPlugin/-/jobs/artifacts/master/download?job=Windows%2064)

2. copy ``OpenRGBEffectsPlugin.dll`` from the 7z file to  ``C:\Users\Username\appdata\roaming\OpenRGB\plugins``

3. Run the latest [Pipeline build of OpenRGB(At least until 0.6 is out)](https://gitlab.com/Calcprogrammer1/OpenRGB/-/jobs/artifacts/master/download?job=Windows%2064)

### **linux**

Make sure `libopenal` is installed first. You can use your package manager to install it (`apt`, `yum`, `pacman`, etc..).

1. download the latest [linux pipeline](https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectsPlugin/-/jobs/artifacts/master/download?job=Linux%2064)

2. Copy ``libOpenRGBEffectsPlugin.so`` from the 7z to ``~/.config/OpenRGB/plugins``

3. Run the latest [Pipeline build of OpenRGB(At least until 0.6 is out)](https://gitlab.com/Calcprogrammer1/OpenRGB/-/jobs/artifacts/master/download?job=Linux%2064%20AppImage)

### **Universal**

After running the OpenRGB build you downloaded from the link the effects tab should show up along the top bar

## What Effects do you have?

* Ambient
* AudioSync
* AudioVisualizer (A port of [Keyboard visualizer](https://gitlab.com/CalcProgrammer1/KeyboardVisualizer))
* Breathing
* Bubbles
* ColorWheelEffect
* FractalMotion
* GifPlayer
* GradientWave
* Hypnotoad
* Lightning
* MotionPoint
* NoiseMap
* RadialRainbow
* Rain
* RainbowWave
* Sequence
* SpectrumCycling
* Stack
* StarryNight
* Visor
* Wavy

## Common Issues

### OpenRGB does not start with the plugin

OpenRGB and EffectsPLugin are under heavy development, make sure to run latest pipeline from both.

### My CPU usage is really high

There are 3 Effects that cause this:

* Audio Visualizer
* Audio Sync
* Ambient

Both of the audio effects will spike in CPU usage when opening an [S/PDIF device](https://en.wikipedia.org/wiki/S/PDIF) for reasons we still don't know. This can be fixed by switching off of the spdif device or stopping the effect

The very nature of ambient is to do massive calulation to get the average or most common color on screen. Scaling uses a lot of CPU so once you have selected your portion of the screen it is recommended that you hide the preview

## Contributing

Please read the [contributing guide](./CONTRIBUTING.md) if you want to add effects or bring new features.

## Donations

This plugin is free and always will be but donations are still nice to get

* CoffeeIsLife

  * Venmo (@joel-fankhauser-2)
