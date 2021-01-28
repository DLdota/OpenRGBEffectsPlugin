QT +=                  \
    gui                \
    widgets            \
    core               \

TEMPLATE = lib
DEFINES += ORGBEFFECTPLUGIN_LIBRARY

CONFIG += c++11


#-----------------------------------------------------------------------------------------------#
# OpenRGB Plugin SDK                                                                            #
#-----------------------------------------------------------------------------------------------#
INCLUDEPATH +=                                                                                  \
    OpenRGB/                                                                                    \
    OpenRGB/i2c_smbus                                                                           \
    OpenRGB/net_port                                                                            \
    OpenRGB/RGBController                                                                       \
    OpenRGB/dependencies/json                                                                   \

HEADERS +=                                                                                      \
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


#-----------------------------------------------------------------------------------------------#
# GUI and misc                                                                                  #
#-----------------------------------------------------------------------------------------------#
INCLUDEPATH += \
    Dependencies/                                                                               \
    Dependencies/HSV/                                                                           \
    Dependencies/ColorWheel/                                                                    \

HEADERS +=                                                                                      \
    ORGBEffectPlugin.h                                                                          \
    OpenRGBEffectTab.h                                                                          \
    OpenRGBEffectPage.h                                                                         \
    Dependencies/HSV/hsv.h                                                                      \
    Dependencies/ColorWheel/ColorWheel.h                                                        \

SOURCES +=                                                                                      \
    ORGBEffectPlugin.cpp                                                                        \
    OpenRGBEffectTab.cpp                                                                        \
    OpenRGBEffectPage.cpp                                                                       \
    Dependencies/HSV/hsv.cpp                                                                    \
    Dependencies/ColorWheel/ColorWheel.cpp                                                      \


#-----------------------------------------------------------------------------------------------#
# Effects                                                                                       #
#-----------------------------------------------------------------------------------------------#
INCLUDEPATH +=                                                                                  \
    Effects/                                                                                    \
    Effects/Breathing/                                                                          \
    Effects/RainbowWave/                                                                        \
    Effects/StarryNight/                                                                        \
    Effects/SpectrumCycling/                                                                    \
    Effects/GradientCycling/                                                                    \

SOURCES +=                                                                                      \
    Effects/Rain/Rain.cpp                                                                       \
    Effects/Breathing/Breathing.cpp                                                             \
    Effects/StarryNight/StarryNight.cpp                                                         \
    Effects/RainbowWave/RainbowWave.cpp                                                         \
    Effects/GradientCycling/GradientCycling.cpp                                                 \
    Effects/SpectrumCycling/SpectrumCycling.cpp                                                 \

HEADERS +=                                                                                      \
    Effects/RGBEffect.h                                                                         \
    Effects/Rain/Rain.h                                                                         \
    Effects/Breathing/Breathing.h                                                               \
    Effects/StarryNight/StarryNight.h                                                           \
    Effects/RainbowWave/RainbowWave.h                                                           \
    Effects/GradientCycling/GradientCycling.h                                                   \
    Effects/SpectrumCycling/SpectrumCycling.h                                                   \


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
