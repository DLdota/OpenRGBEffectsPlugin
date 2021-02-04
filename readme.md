# Effects

## What is this?

This is a plugin for [OpenRGB](https://gitlab.com/CalcProgrammer1/OpenRGB) that provides various Effects that can be synced across devices

## How do I install it?

The installation process is very simple

1. Download the [Latest release](https://gitlab.com/herosilas12/OpenRGB-Plugins/-/commits/Effects) artifacts for your OS (The green checkmark to the right side)

2. Copy the file to the config folder for your specific OS

   * ``C:\Users\Username\appdata\roaming\OpenRGB\plugins`` for windows

   * ``~/.config/OpenRGB/plugins`` for linux

   2.1 You may need to create the ``plugins`` folder (Keeping in mind that it is case sensitve)

3. After running OpenRGB it should show up along the top tab bar

If it doesn't show up then please make sure you have the latest pipeline build of OpenRGB as well

## Why isn't an Effect working on my device?

The way device/zone selection works prevents a device being in use by 2 Effects

If you check a device in Effect 1 and then go to Effect 2 and press start it won't do anything because Effect 1 has the device

You will need to go back to Effect 1 and uncheck the device. Then go to Effect 2 and reselect the device

If you don't know which Effect has the device/zone then you can hover over the checkbox and it will give you a tooltip telling you where it is