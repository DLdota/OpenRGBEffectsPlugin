QT +=               \
    gui             \
    widgets         \
    core            \

TEMPLATE = lib
DEFINES += ORGBEXAMPLEPLUGIN_LIBRARY

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ORGBExamplePlugin.cpp

HEADERS += \
    ORGBExamplePluginInterface.h   \
    ORGBExamplePlugin.h            \
    Dependencies/ResourceManager.h \
    Dependencies/RGBController.h   \
    Dependencies/NetworkClient.h   \
    Dependencies/NetworkServer.h   \
    Dependencies/SettingsManager.h \
    Dependencies/NetworkProtocol.h \
    Dependencies/net_port.h        \
    Dependencies/json.hpp          \

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
