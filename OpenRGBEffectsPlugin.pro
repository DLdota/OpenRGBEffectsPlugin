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
    SHADERS_README=\\"\"\"https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectsPlugin/-/blob/master/Effects/Shaders/README.md\\"\"\"                                               \

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
    OpenRGB/Colors.h                                                                            \
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
    OpenRGB/dependencies/ColorWheel/ColorWheel.h                                                \
    OpenRGB/qt/hsv.h                                                                            \


SOURCES +=                                                                                      \
    OpenRGB/RGBController/RGBController.cpp                                                     \
    OpenRGB/dependencies/ColorWheel/ColorWheel.cpp                                              \
    OpenRGB/qt/hsv.cpp                                                                          \


#-----------------------------------------------------------------------------------------------#
# QCodeEditor                                                                                   #
#-----------------------------------------------------------------------------------------------#
INCLUDEPATH +=                                                                                  \
    Dependencies/QCodeEditor/include                                                            \
    Dependencies/QCodeEditor/include/internal                                                   \

HEADERS +=                                                                                      \
    Dependencies/QCodeEditor/include/internal/QCodeEditor.hpp                                   \
    Dependencies/QCodeEditor/include/internal/QGLSLCompleter.hpp                                \
    Dependencies/QCodeEditor/include/internal/QHighlightRule.hpp                                \
    Dependencies/QCodeEditor/include/internal/QLineNumberArea.hpp                               \
    Dependencies/QCodeEditor/include/internal/QPythonCompleter.hpp                              \
    Dependencies/QCodeEditor/include/internal/QSyntaxStyle.hpp                                  \
    Dependencies/QCodeEditor/include/internal/QCXXHighlighter.hpp                               \
    Dependencies/QCodeEditor/include/internal/QGLSLHighlighter.hpp                              \
    Dependencies/QCodeEditor/include/internal/QJSONHighlighter.hpp                              \
    Dependencies/QCodeEditor/include/internal/QLuaCompleter.hpp                                 \
    Dependencies/QCodeEditor/include/internal/QPythonHighlighter.hpp                            \
    Dependencies/QCodeEditor/include/internal/QXMLHighlighter.hpp                               \
    Dependencies/QCodeEditor/include/internal/QHighlightBlockRule.hpp                           \
    Dependencies/QCodeEditor/include/internal/QLanguage.hpp                                     \
    Dependencies/QCodeEditor/include/internal/QLuaHighlighter.hpp                               \
    Dependencies/QCodeEditor/include/internal/QStyleSyntaxHighlighter.hpp                       \
    Dependencies/QCodeEditor/include/internal/QJSHighlighter.hpp                                \
    Dependencies/QCodeEditor/include/internal/QJavaHighlighter.hpp                              \

SOURCES +=                                                                                      \
    Dependencies/QCodeEditor/src/internal/QCodeEditor.cpp                                       \
    Dependencies/QCodeEditor/src/internal/QGLSLCompleter.cpp                                    \
    Dependencies/QCodeEditor/src/internal/QLanguage.cpp                                         \
    Dependencies/QCodeEditor/src/internal/QLuaHighlighter.cpp                                   \
    Dependencies/QCodeEditor/src/internal/QStyleSyntaxHighlighter.cpp                           \
    Dependencies/QCodeEditor/src/internal/QCXXHighlighter.cpp                                   \
    Dependencies/QCodeEditor/src/internal/QGLSLHighlighter.cpp                                  \
    Dependencies/QCodeEditor/src/internal/QLineNumberArea.cpp                                   \
    Dependencies/QCodeEditor/src/internal/QPythonCompleter.cpp                                  \
    Dependencies/QCodeEditor/src/internal/QSyntaxStyle.cpp                                      \
    Dependencies/QCodeEditor/src/internal/QJSONHighlighter.cpp                                  \
    Dependencies/QCodeEditor/src/internal/QLuaCompleter.cpp                                     \
    Dependencies/QCodeEditor/src/internal/QPythonHighlighter.cpp                                \
    Dependencies/QCodeEditor/src/internal/QXMLHighlighter.cpp                                   \
    Dependencies/QCodeEditor/src/internal/QJSHighlighter.cpp                                    \
    Dependencies/QCodeEditor/src/internal/QJavaHighlighter.cpp                                  \
    Dependencies/QCodeEditor/src/internal/QtCompat.hpp                                          \

RESOURCES +=                                                                                    \
    Dependencies/QCodeEditor/resources/qcodeeditor_resources.qrc

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
    Dependencies/SimplexNoise/src                                                               \

SOURCES +=                                                                                      \
    Dependencies/SimplexNoise/src/SimplexNoise.cpp                                              \

HEADERS +=                                                                                      \
    Dependencies/SimplexNoise/src/SimplexNoise.h                                                \

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
    ColorsPicker.h                                                                              \
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
    ScreenRecorder.h                                                                            \
    SaveProfilePopup.h                                                                          \
    QTooltipedSlider.h                                                                          \
    EffectSearch.h                                                                              \

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
    ColorsPicker.cpp                                                                            \
    PluginInfo.cpp                                                                              \
    ZoneListItem.cpp                                                                            \
    PreviewWidget.cpp                                                                           \
    AudioManager.cpp                                                                            \
    ScreenRecorder.cpp                                                                          \
    SaveProfilePopup.cpp                                                                        \
    QTooltipedSlider.cpp                                                                        \
    EffectSearch.cpp                                                                            \

FORMS +=                                                                                        \
    ControllerZoneList.ui                                                                       \
    DeviceList.ui                                                                               \
    DeviceListItem.ui                                                                           \
    EffectList.ui                                                                               \
    EffectSearch.ui                                                                             \
    EffectTabHeader.ui                                                                          \
    Effects/Shaders/NewShaderPassTabHeader.ui                                                   \
    Effects/Shaders/ShaderFileTabHeader.ui                                                      \
    Effects/Shaders/ShaderPassEditor.ui                                                         \
    LivePreviewController.ui                                                                    \
    OpenRGBEffectPage.ui                                                                        \
    OpenRGBEffectTab.ui                                                                         \
    ColorPicker.ui                                                                              \
    ColorsPicker.ui                                                                             \
    PluginInfo.ui                                                                               \
    SaveProfilePopup.ui                                                                         \
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
    Effects/Ambient/RectangleSelector.cpp                                                       \
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
    Effects/MotionPoints/MotionPoints.cpp                                                       \
    Effects/NoiseMap/NoiseMap.cpp                                                               \
    Effects/RadialRainbow/RadialRainbow.cpp                                                     \
    Effects/Sequence/Sequence.cpp                                                               \
    Effects/Stack/Stack.cpp                                                                     \
    Effects/GifPlayer/GifPlayer.cpp                                                             \
    Effects/SmoothBlink/SmoothBlink.cpp                                                         \
    Effects/BouncingBall/BouncingBallEffect.cpp                                                 \
    Effects/BouncingBall/BouncingBallSimulation.cpp                                             \
    Effects/AudioSine/AudioSine.cpp                                                             \
    Effects/SwirlCircles/SwirlCircles.cpp                                                       \
    Effects/SwirlCirclesAudio/SwirlCirclesAudio.cpp                                             \
    Effects/CrossingBeams/CrossingBeams.cpp                                                     \
    Effects/RotatingBeam/RotatingBeam.cpp                                                       \
    Effects/AudioStar/AudioStar.cpp                                                             \
    Effects/Sunrise/Sunrise.cpp                                                                 \
    Effects/Marquee/Marquee.cpp                                                                 \
    Effects/Comet/Comet.cpp                                                                     \
    Effects/Fill/Fill.cpp                                                                       \
    Effects/Swap/Swap.cpp                                                                       \
    Effects/CustomMarquee/CustomMarquee.cpp                                                     \
    Effects/Mosaic/Mosaic.cpp                                                                   \
    Effects/Bloom/Bloom.cpp                                                                     \
    Effects/Spiral/Spiral.cpp                                                                   \
    Effects/RotatingRainbow/RotatingRainbow.cpp                                                 \
    Effects/DoubleRotatingRainbow/DoubleRotatingRainbow.cpp                                     \
    Effects/Layers/Layers.cpp                                                                   \
    Effects/Layers/LayerEntry.cpp                                                               \
    Effects/Layers/LayerGroupEntry.cpp                                                          \
    Effects/Mask/Mask.cpp                                                                       \
    Effects/MovingPanes/MovingPanes.cpp                                                         \
    Effects/BreathingCircle/BreathingCircle.cpp                                                 \
    Effects/AudioParty/AudioParty.cpp                                                           \    
    Effects/CustomGradientWave/CustomGradientWave.cpp                                           \
    Effects/ZigZag/ZigZag.cpp                                                                   \
    Effects/Shaders/NewShaderPassTabHeader.cpp                                                  \
    Effects/Shaders/ShaderFileTabHeader.cpp                                                     \
    Effects/Shaders/ShaderPass.cpp                                                              \
    Effects/Shaders/ShaderPassData.cpp                                                          \
    Effects/Shaders/ShaderPassEditor.cpp                                                        \
    Effects/Shaders/ShaderProgram.cpp                                                           \
    Effects/Shaders/Shaders.cpp                                                                 \
    Effects/Shaders/ShaderRenderer.cpp                                                          \
    Effects/Shaders/GLSLHighlighter.cpp                                                         \
    Effects/Shaders/GLSLCodeEditor.cpp                                                          \
    Effects/Clock/Clock.cpp                                                                     \
    Effects/CustomBlink/CustomBlink.cpp                                                         \
    Effects/AudioVUMeter/AudioVUMeter.cpp                                                       \

HEADERS +=                                                                                      \
    Effects/RGBEffect.h                                                                         \
    Effects/Rain/Rain.h                                                                         \
    Effects/Wavy/Wavy.h                                                                         \
    Effects/Visor/Visor.h                                                                       \
    Effects/Ambient/Ambient.h                                                                   \
    Effects/Ambient/RectangleSelector.h                                                         \
    Effects/AudioSync/AudioSync.h                                                               \
    Effects/Breathing/Breathing.h                                                               \
    Effects/RainbowWave/RainbowWave.h                                                           \
    Effects/StarryNight/StarryNight.h                                                           \
    Effects/GradientWave/GradientWave.h                                                         \
    Effects/SpectrumCycling/SpectrumCycling.h                                                   \
    Effects/AudioVisualizer/AudioVisualizer.h                                                   \
    Effects/Lightning/Lightning.h                                                               \
    Effects/Bubbles/Bubbles.h                                                                   \
    Effects/ColorWheelEffect/ColorWheelEffect.h                                                 \
    Effects/EffectRegisterer.h                                                                  \
    Effects/FractalMotion/FractalMotion.h                                                       \
    Effects/Hypnotoad/Hypnotoad.h                                                               \
    Effects/MotionPoint/MotionPoint.h                                                           \
    Effects/MotionPoints/MotionPoints.h                                                         \
    Effects/NoiseMap/NoiseMap.h                                                                 \
    Effects/RadialRainbow/RadialRainbow.h                                                       \
    Effects/Sequence/Sequence.h                                                                 \
    Effects/Stack/Stack.h                                                                       \
    Effects/GifPlayer/GifPlayer.h                                                               \
    Effects/SmoothBlink/SmoothBlink.h                                                           \
    Effects/BouncingBall/BouncingBallEffect.h                                                   \
    Effects/BouncingBall/BouncingBallSimulation.h                                               \
    Effects/AudioSine/AudioSine.h                                                               \
    Effects/SwirlCircles/SwirlCircles.h                                                         \
    Effects/SwirlCirclesAudio/SwirlCirclesAudio.h                                               \
    Effects/CrossingBeams/CrossingBeams.h                                                       \
    Effects/RotatingBeam/RotatingBeam.h                                                         \
    Effects/AudioStar/AudioStar.h                                                               \
    Effects/Sunrise/Sunrise.h                                                                   \
    Effects/Marquee/Marquee.h                                                                   \
    Effects/Comet/Comet.h                                                                       \
    Effects/Fill/Fill.h                                                                         \
    Effects/Swap/Swap.h                                                                         \
    Effects/CustomMarquee/CustomMarquee.h                                                       \
    Effects/Mosaic/Mosaic.h                                                                     \
    Effects/Bloom/Bloom.h                                                                       \
    Effects/Spiral/Spiral.h                                                                     \
    Effects/RotatingRainbow/RotatingRainbow.h                                                   \
    Effects/DoubleRotatingRainbow/DoubleRotatingRainbow.h                                       \
    Effects/Layers/Layers.h                                                                     \
    Effects/Layers/LayerEntry.h                                                                 \
    Effects/Layers/LayerGroupEntry.h                                                            \
    Effects/Mask/Mask.h                                                                         \
    Effects/MovingPanes/MovingPanes.h                                                           \
    Effects/BreathingCircle/BreathingCircle.h                                                   \
    Effects/AudioParty/AudioParty.h                                                             \    
    Effects/CustomGradientWave/CustomGradientWave.h                                             \
    Effects/ZigZag/ZigZag.h                                                                     \
    Effects/Shaders/NewShaderPassTabHeader.h                                                    \
    Effects/Shaders/ShaderFileTabHeader.h                                                       \
    Effects/Shaders/ShaderPass.h                                                                \
    Effects/Shaders/ShaderPassData.h                                                            \
    Effects/Shaders/ShaderPassEditor.h                                                          \
    Effects/Shaders/ShaderProgram.h                                                             \
    Effects/Shaders/Shaders.h                                                                   \
    Effects/Shaders/ShaderRenderer.h                                                            \
    Effects/Shaders/GLSLHighlighter.h                                                           \
    Effects/Shaders/GLSLCodeEditor.h                                                            \
    Effects/Clock/Clock.h                                                                       \
    Effects/CustomBlink/CustomBlink.h                                                           \
    Effects/AudioVUMeter/AudioVUMeter.h                                                         \

FORMS +=                                                                                        \
    Effects/Rain/Rain.ui                                                                        \
    Effects/Wavy/Wavy.ui                                                                        \
    Effects/AudioVisualizer/AudioVisualizer.ui                                                  \
    Effects/Bubbles/Bubbles.ui                                                                  \
    Effects/ColorWheelEffect/ColorWheelEffect.ui                                                \
    Effects/FractalMotion/FractalMotion.ui                                                      \
    Effects/Hypnotoad/Hypnotoad.ui                                                              \
    Effects/MotionPoint/MotionPoint.ui                                                          \
    Effects/MotionPoints/MotionPoints.ui                                                        \
    Effects/NoiseMap/NoiseMap.ui                                                                \
    Effects/RadialRainbow/RadialRainbow.ui                                                      \
    Effects/Sequence/Sequence.ui                                                                \
    Effects/Stack/Stack.ui                                                                      \
    Effects/GifPlayer/GifPlayer.ui                                                              \
    Effects/SmoothBlink/SmoothBlink.ui                                                          \
    Effects/BouncingBall/BouncingBallEffect.ui                                                  \
    Effects/AudioSine/AudioSine.ui                                                              \
    Effects/SwirlCircles/SwirlCircles.ui                                                        \
    Effects/SwirlCirclesAudio/SwirlCirclesAudio.ui                                              \
    Effects/CrossingBeams/CrossingBeams.ui                                                      \
    Effects/RotatingBeam/RotatingBeam.ui                                                        \
    Effects/AudioStar/AudioStar.ui                                                              \
    Effects/Sunrise/Sunrise.ui                                                                  \
    Effects/Marquee/Marquee.ui                                                                  \
    Effects/Comet/Comet.ui                                                                      \
    Effects/Fill/Fill.ui                                                                        \
    Effects/Swap/Swap.ui                                                                        \
    Effects/CustomMarquee/CustomMarquee.ui                                                      \
    Effects/Mosaic/Mosaic.ui                                                                    \
    Effects/Bloom/Bloom.ui                                                                      \
    Effects/Spiral/Spiral.ui                                                                    \
    Effects/RotatingRainbow/RotatingRainbow.ui                                                  \
    Effects/DoubleRotatingRainbow/DoubleRotatingRainbow.ui                                      \
    Effects/Layers/Layers.ui                                                                    \
    Effects/Layers/LayerEntry.ui                                                                \
    Effects/Layers/LayerGroupEntry.ui                                                           \
    Effects/Mask/Mask.ui                                                                        \
    Effects/Ambient/Ambient.ui                                                                  \
    Effects/MovingPanes/MovingPanes.ui                                                          \
    Effects/BreathingCircle/BreathingCircle.ui                                                  \
    Effects/AudioParty/AudioParty.ui                                                            \
    Effects/Shaders/Shaders.ui                                                                  \
    Effects/Shaders/GLSLCodeEditor.ui                                                           \
    Effects/CustomGradientWave/CustomGradientWave.ui                                            \
    Effects/ZigZag/ZigZag.ui                                                                    \
    Effects/Clock/Clock.ui                                                                      \
    Effects/CustomBlink/CustomBlink.ui                                                          \
    Effects/AudioVUMeter/AudioVUMeter.ui                                                        \
    Effects/Lightning/Lightning.ui                                                              \

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
        -lOpenGL32                                                      \
}

win32:contains(QMAKE_TARGET.arch, x86) {
    LIBS +=                                                             \
        -lws2_32                                                        \
        -lole32                                                         \
        -lOpenGL32                                                      \
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
    LIBS += -lopenal
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

RESOURCES += \
    resources.qrc
