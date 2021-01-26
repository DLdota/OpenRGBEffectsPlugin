QT +=                  \
    gui                \
    widgets            \
    core               \

TEMPLATE = lib
DEFINES += ORGBEFFECTPLUGIN_LIBRARY

CONFIG += c++11


INCLUDEPATH +=                                                                                  \
    Dependencies/                                                                               \
    Dependencies/HSV/                                                                           \
    Dependencies/ColorWheel/                                                                    \
    Effects/                                                                                    \
    OpenRGB/                                                                                    \
    OpenRGB/dependencies/json                                                                   \
    OpenRGB/i2c_smbus                                                                           \
    OpenRGB/net_port                                                                            \
    OpenRGB/RGBController                                                                       \

#-----------------------------------------------------------------------------------------------#
# OpenRGB Plugin SDK                                                                            #
#-----------------------------------------------------------------------------------------------#

HEADERS +=                                                                                      \
    Dependencies/ColorWheel/ColorWheel.h                                                        \
    Effects/Breathing/Breathing.h                                                               \
    Effects/GradientCycling/GradientCycling.h                                                   \
    Effects/StarryNight/StarryNight.h                                                           \
    OpenRGB/NetworkClient.h                                                                     \
    OpenRGB/NetworkProtocol.h                                                                   \
    OpenRGB/NetworkServer.h                                                                     \
    OpenRGB/OpenRGBPluginInterface.h                                                            \
    OpenRGB/ProfileManager.h                                                                    \
    OpenRGB/ResourceManager.h                                                                   \
    OpenRGB/SettingsManager.h                                                                   \
    OpenRGB/dependencies/json/json.hpp                                                          \
    OpenRGB/i2c_smbus/i2c_smbus.h                                                               \
    OpenRGB/net_port/net_port.h                                                                 \
    OpenRGB/RGBController/RGBController.h                                                       \
    OpenRGBEffectPage.h                                                                         \
    OpenRGBEffectTab.h

#-----------------------------------------------------------------------------------------------#
# Plugin Source                                                                                 #
#-----------------------------------------------------------------------------------------------#
INCLUDEPATH +=                                                                                  \
    Effects/SpectrumCycling/                                                                    \
    Effects/RainbowWave/                                                                        \
    Effects/StarryNight/                                                                        \
    Effects/GradientCycling/                                                                    \

SOURCES +=                                                                                      \
    Dependencies/ColorWheel/ColorWheel.cpp                                                      \
    Effects/Breathing/Breathing.cpp                                                             \
    Effects/GradientCycling/GradientCycling.cpp                                                 \
    Dependencies//HSV/hsv.cpp                                                                   \
    Effects/RainbowWave/RainbowWave.cpp                                                         \
    Effects/SpectrumCycling/SpectrumCycling.cpp                                                 \
    Effects/StarryNight/StarryNight.cpp                                                         \
    ORGBEffectPlugin.cpp                                                                        \
    OpenRGBEffectPage.cpp                                                                       \
    OpenRGBEffectTab.cpp

HEADERS +=                                                                                      \
    Dependencies//HSV/hsv.h                                                                     \
    ORGBEffectPlugin.h                                                                          \
    Effects/RGBEffect.h                                                                         \
    Effects/SpectrumCycling/SpectrumCycling.h                                                   \
    Effects/RainbowWave/RainbowWave.h                                                           \

win32:CONFIG(debug, debug|release) {
    win32:DESTDIR = debug
}

win32:CONFIG(release, debug|release) {
    win32:DESTDIR = release
}

win32:CONFIG += QTPLUGIN

win32:OBJECTS_DIR = _intermediate_$$DESTDIR/.obj
win32:MOC_DIR     = _intermediate_$$DESTDIR/.moc
win32:RCC_DIR     = _intermediate_$$DESTDIR/.qrc
win32:UI_DIR      = _intermediate_$$DESTDIR/.ui

FORMS += \
    OpenRGBEffectPage.ui \
    OpenRGBEffectTab.ui  \
