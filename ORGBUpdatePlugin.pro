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

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
