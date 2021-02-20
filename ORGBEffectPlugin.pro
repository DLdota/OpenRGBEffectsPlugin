QT +=                  \
    gui                \
    widgets            \
    core               \

win32:CONFIG += QTPLUGIN

TEMPLATE = lib
DEFINES += ORGBEFFECTPLUGIN_LIBRARY

#CONFIG += c++11


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
INCLUDEPATH +=                                                                                  \
    Dependencies/                                                                               \
    Dependencies/HSV/                                                                           \
    Dependencies/ColorWheel/                                                                    \
    Dependencies/ScreenSelection/                                                               \

HEADERS +=                                                                                      \
    ORGBEffectPlugin.h                                                                          \
    OpenRGBEffectTab.h                                                                          \
    OpenRGBEffectPage.h                                                                         \
    Dependencies/HSV/hsv.h                                                                      \
    Dependencies/ColorWheel/ColorWheel.h                                                        \
    Dependencies/ScreenSelection/ScreenSelection.h                                              \

SOURCES +=                                                                                      \
    ORGBEffectPlugin.cpp                                                                        \
    OpenRGBEffectTab.cpp                                                                        \
    OpenRGBEffectPage.cpp                                                                       \
    Dependencies/HSV/hsv.cpp                                                                    \
    Dependencies/ColorWheel/ColorWheel.cpp                                                      \
    Dependencies/ScreenSelection/ScreenSelection.cpp                                            \

FORMS +=                                                                                        \
    OpenRGBEffectPage.ui                                                                        \
    OpenRGBEffectTab.ui                                                                         \

#-----------------------------------------------------------------------------------------------#
# Effects                                                                                       #
#-----------------------------------------------------------------------------------------------#
INCLUDEPATH +=                                                                                  \
    Effects/                                                                                    \
    Effects/Rain/                                                                               \
    Effects/Ambient/                                                                            \
    Effects/Breathing/                                                                          \
    Effects/RainbowWave/                                                                        \
    Effects/StarryNight/                                                                        \
    Effects/SpectrumCycling/                                                                    \
    Effects/GradientCycling/                                                                    \
    Effects/SeesawMotion/                                                                       \
    Effects/AudioVisualizer/                                                                    \
    Effects/AudioSync/                                                                          \

SOURCES +=                                                                                      \
    Effects/Rain/Rain.cpp                                                                       \
    Effects/Ambient/Ambient.cpp                                                                 \
    Effects/Breathing/Breathing.cpp                                                             \
    Effects/StarryNight/StarryNight.cpp                                                         \
    Effects/RainbowWave/RainbowWave.cpp                                                         \
    Effects/GradientCycling/GradientCycling.cpp                                                 \
    Effects/SpectrumCycling/SpectrumCycling.cpp                                                 \
    Effects/SeesawMotion/SeesawMotion.cpp                                                       \
    Effects/AudioVisualizer/AudioVisualizer.cpp                                                 \
    Effects/AudioSync/AudioSync.cpp                                                             \

HEADERS +=                                                                                      \
    Effects/RGBEffect.h                                                                         \
    Effects/Rain/Rain.h                                                                         \
    Effects/Ambient/Ambient.h                                                                   \
    Effects/Breathing/Breathing.h                                                               \
    Effects/StarryNight/StarryNight.h                                                           \
    Effects/RainbowWave/RainbowWave.h                                                           \
    Effects/GradientCycling/GradientCycling.h                                                   \
    Effects/SpectrumCycling/SpectrumCycling.h                                                   \
    Effects/SeesawMotion/SeesawMotion.h                                                         \
    Effects/AudioVisualizer/AudioVisualizer.h                                                   \
    Effects/AudioSync/AudioSync.h                                                               \

#-----------------------------------------------------------------------------------------------#
# AudioManager                                                                                  #
#-----------------------------------------------------------------------------------------------#
INCLUDEPATH +=                                                                                  \
    Dependencies/AudioManager/                                                                  \

HEADERS +=                                                                                      \
    Dependencies/AudioManager/AudioManager.h                                                    \

SOURCES +=                                                                                      \
    Dependencies/AudioManager/AudioManager.cpp                                                  \


#-----------------------------------------------------------------------------------------------#
# Keyboard Visualizer https://gitlab.com/CalcProgrammer1/KeyboardVisualizer                     #
#-----------------------------------------------------------------------------------------------#
INCLUDEPATH +=                                                                                  \
    Dependencies/AudioVisualizer/                                                               \

HEADERS +=                                                                                      \
    Dependencies/AudioVisualizer/Visualizer.h                                                   \
    Dependencies/AudioVisualizer/VisualizerDefines.h                                            \
    Dependencies/AudioVisualizer/AudioVisualizerUi.h                                            \

SOURCES +=                                                                                      \
    Dependencies/AudioVisualizer/AudioVisualizerUi.cpp                                          \
    Dependencies/AudioVisualizer/Visualizer.cpp                                                 \

FORMS +=                                                                                        \
    Dependencies/AudioVisualizer/AudioVisualizerUi.ui                                           \

#-----------------------------------------------------------------------------------------------#
# ctkrangeslider                                                                                #
#-----------------------------------------------------------------------------------------------#
INCLUDEPATH +=                                                                                  \
    Dependencies/ctkrangeslider/                                                                \

SOURCES +=                                                                                      \
    Dependencies/ctkrangeslider/ctkrangeslider.cpp                                              \

HEADERS +=                                                                                      \
    Dependencies/ctkrangeslider/ctkrangeslider.h                                                \

#-----------------------------------------------------------------------------------------------#
# chuck_fft                                                                                     #
#-----------------------------------------------------------------------------------------------#
INCLUDEPATH +=                                                                                  \
    Dependencies/chuck_fft/                                                                     \

SOURCES +=                                                                                      \
    Dependencies/chuck_fft/chuck_fft.c

HEADERS +=                                                                                      \
    Dependencies/chuck_fft/chuck_fft.h                                                          \

#-----------------------------------------------------------------------------------------------#
# Xtensor (For ambient)                                                                         #
#-----------------------------------------------------------------------------------------------#
INCLUDEPATH +=                                                                                  \
    Dependencies/xtensor/                                                                       \

HEADERS +=                                                                                      \
    Dependencies/xtensor/xaccessible.hpp                                                        \
    Dependencies/xtensor/xaccumulator.hpp                                                       \
    Dependencies/xtensor/xadapt.hpp                                                             \
    Dependencies/xtensor/xarray.hpp                                                             \
    Dependencies/xtensor/xassign.hpp                                                            \
    Dependencies/xtensor/xaxis_iterator.hpp                                                     \
    Dependencies/xtensor/xaxis_slice_iterator.hpp                                               \
    Dependencies/xtensor/xbroadcast.hpp                                                         \
    Dependencies/xtensor/xbuffer_adaptor.hpp                                                    \
    Dependencies/xtensor/xbuilder.hpp                                                           \
    Dependencies/xtensor/xchunked_array.hpp                                                     \
    Dependencies/xtensor/xcomplex.hpp                                                           \
    Dependencies/xtensor/xcontainer.hpp                                                         \
    Dependencies/xtensor/xcsv.hpp                                                               \
    Dependencies/xtensor/xdynamic_view.hpp                                                      \
    Dependencies/xtensor/xeval.hpp                                                              \
    Dependencies/xtensor/xexception.hpp                                                         \
    Dependencies/xtensor/xexpression.hpp                                                        \
    Dependencies/xtensor/xexpression_holder.hpp                                                 \
    Dependencies/xtensor/xexpression_traits.hpp                                                 \
    Dependencies/xtensor/xfixed.hpp                                                             \
    Dependencies/xtensor/xfunction.hpp                                                          \
    Dependencies/xtensor/xfunctor_view.hpp                                                      \
    Dependencies/xtensor/xgenerator.hpp                                                         \
    Dependencies/xtensor/xhistogram.hpp                                                         \
    Dependencies/xtensor/xindex_view.hpp                                                        \
    Dependencies/xtensor/xinfo.hpp                                                              \
    Dependencies/xtensor/xio.hpp                                                                \
    Dependencies/xtensor/xiterable.hpp                                                          \
    Dependencies/xtensor/xiterator.hpp                                                          \
    Dependencies/xtensor/xjson.hpp                                                              \
    Dependencies/xtensor/xlayout.hpp                                                            \
    Dependencies/xtensor/xmanipulation.hpp                                                      \
    Dependencies/xtensor/xmasked_view.hpp                                                       \
    Dependencies/xtensor/xmath.hpp                                                              \
    Dependencies/xtensor/xmime.hpp                                                              \
    Dependencies/xtensor/xnoalias.hpp                                                           \
    Dependencies/xtensor/xnorm.hpp                                                              \
    Dependencies/xtensor/xnpy.hpp                                                               \
    Dependencies/xtensor/xoffset_view.hpp                                                       \
    Dependencies/xtensor/xoperation.hpp                                                         \
    Dependencies/xtensor/xoptional.hpp                                                          \
    Dependencies/xtensor/xoptional_assembly.hpp                                                 \
    Dependencies/xtensor/xoptional_assembly_base.hpp                                            \
    Dependencies/xtensor/xoptional_assembly_storage.hpp                                         \
    Dependencies/xtensor/xpad.hpp                                                               \
    Dependencies/xtensor/xrandom.hpp                                                            \
    Dependencies/xtensor/xreducer.hpp                                                           \
    Dependencies/xtensor/xrepeat.hpp                                                            \
    Dependencies/xtensor/xscalar.hpp                                                            \
    Dependencies/xtensor/xsemantic.hpp                                                          \
    Dependencies/xtensor/xset_operation.hpp                                                     \
    Dependencies/xtensor/xshape.hpp                                                             \
    Dependencies/xtensor/xslice.hpp                                                             \
    Dependencies/xtensor/xsort.hpp                                                              \
    Dependencies/xtensor/xstorage.hpp                                                           \
    Dependencies/xtensor/xstrided_view.hpp                                                      \
    Dependencies/xtensor/xstrided_view_base.hpp                                                 \
    Dependencies/xtensor/xstrides.hpp                                                           \
    Dependencies/xtensor/xtensor.hpp                                                            \
    Dependencies/xtensor/xtensor_config.hpp                                                     \
    Dependencies/xtensor/xtensor_forward.hpp                                                    \
    Dependencies/xtensor/xtensor_simd.hpp                                                       \
    Dependencies/xtensor/xutils.hpp                                                             \
    Dependencies/xtensor/xvectorize.hpp                                                         \
    Dependencies/xtensor/xview.hpp                                                              \
    Dependencies/xtensor/xview_utils.hpp                                                        \
    Dependencies/xtensor/xtl/xany.hpp                                                           \
    Dependencies/xtensor/xtl/xbase64.hpp                                                        \
    Dependencies/xtensor/xtl/xbasic_fixed_string.hpp                                            \
    Dependencies/xtensor/xtl/xclosure.hpp                                                       \
    Dependencies/xtensor/xtl/xcomplex.hpp                                                       \
    Dependencies/xtensor/xtl/xcomplex_sequence.hpp                                              \
    Dependencies/xtensor/xtl/xdynamic_bitset.hpp                                                \
    Dependencies/xtensor/xtl/xfunctional.hpp                                                    \
    Dependencies/xtensor/xtl/xhalf_float.hpp                                                    \
    Dependencies/xtensor/xtl/xhalf_float_impl.hpp                                               \
    Dependencies/xtensor/xtl/xhash.hpp                                                          \
    Dependencies/xtensor/xtl/xhierarchy_generator.hpp                                           \
    Dependencies/xtensor/xtl/xiterator_base.hpp                                                 \
    Dependencies/xtensor/xtl/xjson.hpp                                                          \
    Dependencies/xtensor/xtl/xmasked_value.hpp                                                  \
    Dependencies/xtensor/xtl/xmasked_value_meta.hpp                                             \
    Dependencies/xtensor/xtl/xmeta_utils.hpp                                                    \
    Dependencies/xtensor/xtl/xmultimethods.hpp                                                  \
    Dependencies/xtensor/xtl/xoptional.hpp                                                      \
    Dependencies/xtensor/xtl/xoptional_meta.hpp                                                 \
    Dependencies/xtensor/xtl/xoptional_sequence.hpp                                             \
    Dependencies/xtensor/xtl/xplatform.hpp                                                      \
    Dependencies/xtensor/xtl/xproxy_wrapper.hpp                                                 \
    Dependencies/xtensor/xtl/xsequence.hpp                                                      \
    Dependencies/xtensor/xtl/xspan.hpp                                                          \
    Dependencies/xtensor/xtl/xspan_impl.hpp                                                     \
    Dependencies/xtensor/xtl/xsystem.hpp                                                        \
    Dependencies/xtensor/xtl/xtl_config.hpp                                                     \
    Dependencies/xtensor/xtl/xtype_traits.hpp                                                   \
    Dependencies/xtensor/xtl/xvariant.hpp                                                       \
    Dependencies/xtensor/xtl/xvariant_impl.hpp                                                  \
    Dependencies/xtensor/xtl/xvisitor.hpp                                                       \

#-------------------------------------------------------------------#
# Windows GitLab CI Configuration                                   #
#-------------------------------------------------------------------#
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
    LIBS += -lopenal
}

#-----------------------------------------------------------------------#
# MacOS-specific Configuration                                          #
#-----------------------------------------------------------------------#
macx: {
    LIBS += -framework OpenAL
}
