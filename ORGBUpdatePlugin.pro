QT +=                  \
    gui                \
    widgets            \
    core               \
    network            \
    webenginewidgets   \

TEMPLATE = lib
DEFINES += ORGBUPDATEPLUGIN_LIBRARY

CONFIG += c++11

#-----------------------------------------------------------------------------------------------#
# Plugin Source                                                                                 #
#-----------------------------------------------------------------------------------------------#
SOURCES +=                                      \
    ORGBUpdatePlugin.cpp                        \
    OpenRGBUpdateTab.cpp                        \

HEADERS +=                                      \
    ORGBUpdatePlugin.h                          \
    OpenRGBPluginInterface.h \
    OpenRGBUpdateTab.h                          \

FORMS +=                                        \
    ORGBUpdatePlugin.ui                         \

RESOURCES +=                                    \
    resources.qrc                               \


#-----------------------------------------------------------------------------------------------#
# OpenRGB Plugin SDK                                                                            #
#-----------------------------------------------------------------------------------------------#
INCLUDEPATH +=                                                                                  \
    OpenRGB/                                                                                    \
    OpenRGB/i2c_smbus                                                                           \
    OpenRGB/net_port                                                                            \
    OpenRGB/RGBController                                                                       \

HEADERS +=                                                                                      \
    OpenRGB/NetworkClient.h                                                                     \
    OpenRGB/NetworkProtocol.h                                                                   \
    OpenRGB/NetworkServer.h                                                                     \
    OpenRGB/ProfileManager.h                                                                    \
    OpenRGB/ResourceManager.h                                                                   \
    OpenRGB/SettingsManager.h                                                                   \
    OpenRGB/dependencies/json/json.hpp                                                          \
    OpenRGB/i2c_smbus/i2c_smbus.h                                                               \
    OpenRGB/net_port/net_port.h                                                                 \
    OpenRGB/RGBController/RGBController.h                                                       \

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
