QT +=                  \
    gui                \
    widgets            \
    core               \

TEMPLATE = lib
DEFINES += ORGBEFFECTPLUGIN_LIBRARY

CONFIG += c++11


INCLUDEPATH +=                                                                                  \
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
SOURCES +=                                                                                      \
    Effects/SpectrumCycling.cpp                                                                 \
    ORGBEffectPlugin.cpp                                                                        \
    OpenRGBEffectPage.cpp                                                                       \
    OpenRGBEffectTab.cpp

HEADERS +=                                                                                      \
    ORGBEffectPlugin.h                                                                          \
    OpenRGBPluginInterface.h                                                                    \
    Effects/RGBEffect.h                                                                         \
    Effects/SpectrumCycling.h                                                                   \

win32:contains(QMAKE_TARGET.arch, x86_64) {
    LIBS +=                                                                                     \
        -L"$$PWD/openssl/x64/" -llibssl                                                         \
        -L"$$PWD/openssl/x64/" -llibcrypto                                                      \
}

win32:contains(QMAKE_TARGET.arch, x86) {
    LIBS +=                                                                                     \
    -L"$$PWD/openssl/x86/" -llibssl                                                             \
    -L"$$PWD/openssl/x86/" -llibcrypto                                                          \
}

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

win32:contains(QMAKE_TARGET.arch, x86_64) {
    copydata.commands += $(COPY_FILE) \"$$shell_path($$PWD/openssl/x64/libcrypto-1_1-x64.dll            )\" \"$$shell_path($$DESTDIR)\" $$escape_expand(\n\t)
    copydata.commands += $(COPY_FILE) \"$$shell_path($$PWD/openssl/x64/libssl-1_1-x64.dll               )\" \"$$shell_path($$DESTDIR)\" $$escape_expand(\n\t)
    first.depends = $(first) copydata
    export(first.depends)
    export(copydata.commands)
    QMAKE_EXTRA_TARGETS += first copydata
}

win32:contains(QMAKE_TARGET.arch, x86) {
    copydata.commands += $(COPY_FILE) \"$$shell_path($$PWD/openssl/x86/libcrypto-1_1.dll            )\" \"$$shell_path($$DESTDIR)\" $$escape_expand(\n\t)
    copydata.commands += $(COPY_FILE) \"$$shell_path($$PWD/openssl/x86/libssl-1_1.dll               )\" \"$$shell_path($$DESTDIR)\" $$escape_expand(\n\t)
    first.depends = $(first) copydata
    export(first.depends)
    export(copydata.commands)
    QMAKE_EXTRA_TARGETS += first copydata
}

FORMS += \
    OpenRGBEffectPage.ui \
    OpenRGBEffectTab.ui
