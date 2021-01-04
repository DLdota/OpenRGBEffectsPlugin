QT +=                  \
    gui                \
    widgets            \
    core               \
    network            \
    webenginewidgets   \

TEMPLATE = lib
DEFINES += ORGBUPDATEPLUGIN_LIBRARY

CONFIG += c++11

SOURCES +=                                      \
    ORGBUpdatePlugin.cpp                        \
    OpenRGBUpdateTab.cpp                        \

HEADERS +=                                      \
    ORGBPluginInterface.h                       \
    ORGBUpdatePlugin.h                          \
    OpenRGBUpdateTab.h                          \
    Dependencies/ResourceManager.h              \
    Dependencies/IResourceManager.h             \
    Dependencies/SettingsManager.h              \
    Dependencies/i2c_smbus/i2c_smbus.h          \
    Dependencies/RGBController/RGBController.h  \
    Dependencies/json.hpp                       \
    Dependencies/NetworkClient.h                \
    Dependencies/NetworkProtocol.h              \
    Dependencies/NetworkServer.h                \
    Dependencies/ProfileManager.h
FORMS +=                                        \
    ORGBUpdatePlugin.ui                         \

RESOURCES +=                                    \
    resources.qrc                               \

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
