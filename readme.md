# Effects

[![pipeline status](https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectPlugin/badges/master/pipeline.svg)](https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectPlugin/-/commits/master)

## What is this?

This is a plugin for [OpenRGB](https://gitlab.com/CalcProgrammer1/OpenRGB) that provides various Effects that can be synced across devices

## How do I install it?

The installation process is very simple

1. Download the [Latest release](https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectPlugin/-/commits/master) artifacts for your OS (The green checkmark to the right side)

2. Copy the file to the config folder for your specific OS

   * ``OpenRGBEffectPlugin.dll`` to  ``C:\Users\Username\appdata\roaming\OpenRGB\plugins`` for windows

   * ``libOpenRGBEffectPlugin.so`` to ``~/.config/OpenRGB/plugins`` for linux

   2.1 You may need to create the ``plugins`` folder (Keeping in mind that it is case sensitve)

3. After running OpenRGB it should show up along the top tab bar

If it doesn't show up then please make sure you have the latest pipeline build of OpenRGB as well

## What Effects do you have?

* Spectrum Cycling

* Rainbow Wave

* Starry night

* Gradient Cycling

* Breathing

* Rain

* Ambient

* Visor (By Morg)

### Audio Effects (Backend created by Morg)

* Audio Visualizer (A port of [Keyboard visualizer](https://gitlab.com/CalcProgrammer1/KeyboardVisualizer))

* Audio Sync (Morgs implementation of Audio Visualizer)

## Common Issues

### Only 1 effect is working?

The way device/zone selection works prevents a device being in use by 2 Effects

If you check a device in Effect 1 and then go to Effect 2 and press start it won't do anything because Effect 1 has the device

You will need to go back to Effect 1 and uncheck the device. Then go to Effect 2 and reselect the device

If you don't know which Effect has the device/zone then you can hover over the checkbox and it will give you a tooltip telling you where it is

### My CPU usage is really high

There are 3 Effects that cause this:

1. Audio Visualizer

2. Audio Sync

3. Ambient

Both of the audio effects will spike in CPU usage when opening an [S/PDIF device](https://en.wikipedia.org/wiki/S/PDIF) for reasons we still don't know. This can be fixed by switching off of the spdif device or stopping the effect

The very nature of ambient is to do massive calulation to get the average or most common color on screen. Scaling using a lot of CPU so once you have selected your portion of the screen it is recommended that you hide the preview
