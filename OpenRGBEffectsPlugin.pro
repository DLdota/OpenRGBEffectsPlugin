QT += gui widgets core
DEFINES += OPEN_RGB_EFFECTS_PLUGIN_LIBRARY
TEMPLATE = lib

#-----------------------------------------------------------------------------------------------#
# Application Configuration                                                                     #
#-----------------------------------------------------------------------------------------------#
PLUGIN_VERSION     = 0.1

#-----------------------------------------------------------------------------------------------#
# Automatically generated build information                                                     #
#-----------------------------------------------------------------------------------------------#
win32:BUILDDATE = $$system(date /t)
unix:BUILDDATE  = $$system(date -R -d "@${SOURCE_DATE_EPOCH:-$(date +%s)}")
GIT_COMMIT_ID   = $$system(git --git-dir $$_PRO_FILE_PWD_/.git --work-tree $$_PRO_FILE_PWD_ rev-parse HEAD)
GIT_COMMIT_DATE = $$system(git --git-dir $$_PRO_FILE_PWD_/.git --work-tree $$_PRO_FILE_PWD_ show -s --format=%ci HEAD)
GIT_BRANCH      = $$system(git --git-dir $$_PRO_FILE_PWD_/.git --work-tree $$_PRO_FILE_PWD_ rev-parse --abbrev-ref HEAD)

#-----------------------------------------------------------------------------------------------#
# Download links                                                                                #
#-----------------------------------------------------------------------------------------------#
win32:LATEST_BUILD_URL="https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectsPlugin/-/jobs/artifacts/master/download?job=Windows 64"
unix:!macx:LATEST_BUILD_URL="https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectsPlugin/-/jobs/artifacts/master/download?job=Linux 64"

#-----------------------------------------------------------------------------------------------#
# Inject vars in defines                                                                        #
#-----------------------------------------------------------------------------------------------#
DEFINES +=                                                                                      \
    VERSION_STRING=\\"\"\"$$PLUGIN_VERSION\\"\"\"                                               \
    BUILDDATE_STRING=\\"\"\"$$BUILDDATE\\"\"\"                                                  \
    GIT_COMMIT_ID=\\"\"\"$$GIT_COMMIT_ID\\"\"\"                                                 \
    GIT_COMMIT_DATE=\\"\"\"$$GIT_COMMIT_DATE\\"\"\"                                             \
    GIT_BRANCH=\\"\"\"$$GIT_BRANCH\\"\"\"                                                       \
    LATEST_BUILD_URL=\\"\"\"$$LATEST_BUILD_URL\\"\"\"                                           \

#-----------------------------------------------------------------------------------------------#
# OpenRGB Plugin SDK                                                                            #
#-----------------------------------------------------------------------------------------------#
INCLUDEPATH +=                                                                                  \
    OpenRGB                                                                                     \
    OpenRGB/i2c_smbus                                                                           \
    OpenRGB/net_port                                                                            \
    OpenRGB/RGBController                                                                       \
    OpenRGB/dependencies/json                                                                   \
    OpenRGB/dependencies/ColorWheel                                                             \
    OpenRGB/qt                                                                                  \

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
    OpenRGB/filesystem.h                                                                        \
    OpenRGB/dependencies/ColorWheel/ColorWheel.h                                                \
    OpenRGB/qt/hsv.h                                                                            \


SOURCES +=                                                                                      \
    OpenRGB/RGBController/RGBController.cpp                                                     \
    OpenRGB/dependencies/ColorWheel/ColorWheel.cpp                                              \
    OpenRGB/qt/hsv.cpp                                                                          \

#-----------------------------------------------------------------------------------------------#
# ctkrangeslider                                                                                #
#-----------------------------------------------------------------------------------------------#
INCLUDEPATH +=                                                                                  \
    Dependencies/ctkrangeslider                                                                 \

HEADERS +=                                                                                      \
    Dependencies/ctkrangeslider/ctkrangeslider.h                                                \

SOURCES +=                                                                                      \
    Dependencies/ctkrangeslider/ctkrangeslider.cpp                                              \


#-----------------------------------------------------------------------------------------------#
# chuck_fft                                                                                     #
#-----------------------------------------------------------------------------------------------#
INCLUDEPATH +=                                                                                  \
    Dependencies/chuck_fft                                                                      \

SOURCES +=                                                                                      \
    Dependencies/chuck_fft/chuck_fft.c

HEADERS +=                                                                                      \
    Dependencies/chuck_fft/chuck_fft.h                                                          \

#-----------------------------------------------------------------------------------------------#
# SimplexNoise                                                                                  #
#-----------------------------------------------------------------------------------------------#
INCLUDEPATH +=                                                                                  \
    Dependencies/SimplexNoise                                                                   \

SOURCES +=                                                                                      \
    Dependencies/SimplexNoise/SimplexNoise.cpp                                                  \

HEADERS +=                                                                                      \
    Dependencies/SimplexNoise/SimplexNoise.h                                                    \

#-----------------------------------------------------------------------------------------------#
# Xtensor (For ambient)                                                                         #
#-----------------------------------------------------------------------------------------------#
INCLUDEPATH +=                                                                                  \
    Dependencies/xtensor                                                                        \

#-----------------------------------------------------------------------------------------------#
# GUI and misc                                                                                  #
#-----------------------------------------------------------------------------------------------#
HEADERS +=                                                                                      \
    OpenRGBEffectsPlugin.h                                                                      \
    OpenRGBEffectTab.h                                                                          \
    OpenRGBEffectPage.h                                                                         \
    OpenRGBEffectSettings.h                                                                     \
    EffectManager.h                                                                             \
    ColorPicker.h                                                                               \
    ColorUtils.h                                                                                \
    ControllerZone.h                                                                            \
    ControllerZoneList.h                                                                        \
    DeviceList.h                                                                                \
    DeviceListItem.h                                                                            \
    EffectList.h                                                                                \
    EffectTabHeader.h                                                                           \
    LivePreviewController.h                                                                     \
    PluginInfo.h                                                                                \
    PreviewWidget.h                                                                             \
    ZoneListItem.h                                                                              \
    AudioManager.h                                                                              \

SOURCES +=                                                                                      \
    ControllerZoneList.cpp                                                                      \
    DeviceList.cpp                                                                              \
    DeviceListItem.cpp                                                                          \
    EffectList.cpp                                                                              \
    EffectTabHeader.cpp                                                                         \
    LivePreviewController.cpp                                                                   \
    OpenRGBEffectsPlugin.cpp                                                                    \
    OpenRGBEffectTab.cpp                                                                        \
    OpenRGBEffectPage.cpp                                                                       \
    OpenRGBEffectSettings.cpp                                                                   \
    EffectManager.cpp                                                                           \
    ColorPicker.cpp                                                                             \
    PluginInfo.cpp                                                                              \
    ZoneListItem.cpp                                                                            \
    PreviewWidget.cpp                                                                           \
    AudioManager.cpp                                                                            \

FORMS +=                                                                                        \
    ControllerZoneList.ui                                                                       \
    DeviceList.ui                                                                               \
    DeviceListItem.ui                                                                           \
    EffectList.ui                                                                               \
    EffectTabHeader.ui                                                                          \
    LivePreviewController.ui                                                                    \
    OpenRGBEffectPage.ui                                                                        \
    OpenRGBEffectTab.ui                                                                         \
    ColorPicker.ui                                                                              \
    PluginInfo.ui                                                                               \
    ZoneListItem.ui

#-----------------------------------------------------------------------------------------------#
# Effects                                                                                       #
#-----------------------------------------------------------------------------------------------#
INCLUDEPATH +=                                                                                  \
    Effects                                                                                     \

SOURCES +=                                                                                      \
    Effects/Rain/Rain.cpp                                                                       \
    Effects/Wavy/Wavy.cpp                                                                       \
    Effects/Visor/Visor.cpp                                                                     \
    Effects/Ambient/Ambient.cpp                                                                 \
    Effects/AudioSync/AudioSync.cpp                                                             \
    Effects/Breathing/Breathing.cpp                                                             \
    Effects/RainbowWave/RainbowWave.cpp                                                         \
    Effects/StarryNight/StarryNight.cpp                                                         \
    Effects/GradientWave/GradientWave.cpp                                                       \
    Effects/AudioVisualizer/AudioVisualizer.cpp                                                 \
    Effects/Lightning/Lightning.cpp                                                             \
    Effects/SpectrumCycling/SpectrumCycling.cpp                                                 \
    Effects/Bubbles/Bubbles.cpp                                                                 \
    Effects/ColorWheelEffect/ColorWheelEffect.cpp                                               \
    Effects/FractalMotion/FractalMotion.cpp                                                     \
    Effects/Hypnotoad/Hypnotoad.cpp                                                             \
    Effects/MotionPoint/MotionPoint.cpp                                                         \
    Effects/NoiseMap/NoiseMap.cpp                                                               \
    Effects/RadialRainbow/RadialRainbow.cpp                                                     \
    Effects/Sequence/Sequence.cpp                                                               \
    Effects/Stack/Stack.cpp                                                                     \
    Effects/GifPlayer/GifPlayer.cpp                                                             \
    Effects/Ambient/ScreenSelection.cpp                                                         \
    Effects/SmoothBlink/SmoothBlink.cpp                                                         \
    Effects/BouncingBall/BouncingBallEffect.cpp                                                 \
    Effects/BouncingBall/BouncingBallSimulation.cpp                                             \
    Effects/AudioSine/AudioSine.cpp                                                             \

HEADERS +=                                                                                      \
    Effects/RGBEffect.h                                                                         \
    Effects/Rain/Rain.h                                                                         \
    Effects/Wavy/Wavy.h                                                                         \
    Effects/Visor/Visor.h                                                                       \
    Effects/Ambient/Ambient.h                                                                   \
    Effects/AudioSync/AudioSync.h                                                               \
    Effects/Breathing/Breathing.h                                                               \
    Effects/RainbowWave/RainbowWave.h                                                           \
    Effects/StarryNight/StarryNight.h                                                           \
    Effects/GradientWave/GradientWave.h                                                         \
    Effects/SpectrumCycling/SpectrumCycling.h                                                   \
    Effects/AudioVisualizer/AudioVisualizer.h                                                   \
    Effects/Lightning/Lightning.h                                                               \
    Effects/Ambient/ScreenSelection.h                                                           \
    Effects/Bubbles/Bubbles.h                                                                   \
    Effects/ColorWheelEffect/ColorWheelEffect.h                                                 \
    Effects/EffectRegisterer.h                                                                  \
    Effects/FractalMotion/FractalMotion.h                                                       \
    Effects/Hypnotoad/Hypnotoad.h                                                               \
    Effects/MotionPoint/MotionPoint.h                                                           \
    Effects/NoiseMap/NoiseMap.h                                                                 \
    Effects/RadialRainbow/RadialRainbow.h                                                       \
    Effects/Sequence/Sequence.h                                                                 \
    Effects/Stack/Stack.h                                                                       \
    Effects/GifPlayer/GifPlayer.h                                                               \
    Effects/SmoothBlink/SmoothBlink.h                                                           \
    Effects/BouncingBall/BouncingBallEffect.h                                                   \
    Effects/BouncingBall/BouncingBallSimulation.h                                               \
    Effects/AudioSine/AudioSine.h                                                               \

FORMS +=                                                                                        \
    Effects/Wavy/Wavy.ui                                                                        \
    Effects/AudioVisualizer/AudioVisualizer.ui                                                  \
    Effects/Bubbles/Bubbles.ui                                                                  \
    Effects/ColorWheelEffect/ColorWheelEffect.ui                                                \
    Effects/FractalMotion/FractalMotion.ui                                                      \
    Effects/Hypnotoad/Hypnotoad.ui                                                              \
    Effects/MotionPoint/MotionPoint.ui                                                          \
    Effects/NoiseMap/NoiseMap.ui                                                                \
    Effects/RadialRainbow/RadialRainbow.ui                                                      \
    Effects/Sequence/Sequence.ui                                                                \
    Effects/Stack/Stack.ui                                                                      \
    Effects/GifPlayer/GifPlayer.ui                                                              \
    Effects/SmoothBlink/SmoothBlink.ui                                                          \
    Effects/BouncingBall/BouncingBallEffect.ui                                                  \
    Effects/AudioSine/AudioSine.ui                                                              \


#-----------------------------------------------------------------------------------------------#
# Windows  Configuration                                                                        #
#-----------------------------------------------------------------------------------------------#
win32:CONFIG += QTPLUGIN c++17

win32:CONFIG(debug, debug|release) {
    win32:DESTDIR = debug
}

win32:CONFIG(release, debug|release) {
    win32:DESTDIR = release
}

win32:OBJECTS_DIR = _intermediate_$$DESTDIR/.obj
win32:MOC_DIR     = _intermediate_$$DESTDIR/.moc
win32:RCC_DIR     = _intermediate_$$DESTDIR/.qrc
win32:UI_DIR      = _intermediate_$$DESTDIR/.ui

win32:contains(QMAKE_TARGET.arch, x86_64) {
    LIBS +=                                                             \
        -lws2_32                                                        \
        -lole32                                                         \
}

win32:contains(QMAKE_TARGET.arch, x86) {
    LIBS +=                                                             \
        -lws2_32                                                        \
        -lole32                                                         \
}

win32:DEFINES +=                                                        \
    _MBCS                                                               \
    WIN32                                                               \
    _CRT_SECURE_NO_WARNINGS                                             \
    _WINSOCK_DEPRECATED_NO_WARNINGS                                     \
    WIN32_LEAN_AND_MEAN                                                 \

#-----------------------------------------------------------------------#
# Linux-specific Configuration                                          #
#-----------------------------------------------------------------------#
unix:!macx {
    LIBS += -lopenal -lstdc++fs
    QMAKE_CXXFLAGS += -std=c++17
}

#-----------------------------------------------------------------------#
# MacOS-specific Configuration                                          #
#-----------------------------------------------------------------------#
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15

macx: {
    CONFIG += c++17
    LIBS += -framework OpenAL
}
