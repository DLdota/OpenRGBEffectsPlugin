# Effects

[![pipeline status](https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectPlugin/badges/master/pipeline.svg)](https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectPlugin/-/commits/master)

## What is this?

This is a plugin for [OpenRGB](https://gitlab.com/CalcProgrammer1/OpenRGB) that provides various Effects that can be synced across devices

## How do I install it?

### **windows**

1. download the latest [Windows pipeline](https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectPlugin/-/jobs/artifacts/master/download?job=Windows%2064)

2. copy ``OpenRGBEffectPlugin.dll`` from the 7z file to  ``C:\Users\Username\appdata\roaming\OpenRGB\plugins``

3. Run the latest [Pipeline build of OpenRGB(At least until 0.6 is out)](https://gitlab.com/Calcprogrammer1/OpenRGB/-/jobs/artifacts/master/download?job=Windows%2064)

### **linux**

Make sure `libopenal` is installed first. You can use your package manager to install it (`apt`, `yum`, `pacman`, etc..).

1. download the latest [linux pipeline](https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectPlugin/-/jobs/artifacts/master/download?job=Linux%2064)

2. Copy ``libOpenRGBEffectPlugin.so`` from the 7z to ``~/.config/OpenRGB/plugins``

3. Run the latest [Pipeline build of OpenRGB(At least until 0.6 is out)](https://gitlab.com/Calcprogrammer1/OpenRGB/-/jobs/artifacts/master/download?job=Linux%2064%20AppImage)

### **Universal**

After running the OpenRGB build you downloaded from the link the effects tab should show up along the top bar

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

The very nature of ambient is to do massive calulation to get the average or most common color on screen. Scaling uses a lot of CPU so once you have selected your portion of the screen it is recommended that you hide the preview
