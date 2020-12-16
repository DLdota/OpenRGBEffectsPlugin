QT +=                  \
    gui                \
    widgets            \
    core               \
    network            \
    webenginewidgets   \

TEMPLATE = lib
DEFINES += ORGBEXAMPLEPLUGIN_LIBRARY

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES +=                         \
    ORGBUpdatePlugin.cpp           \

HEADERS +=                         \
    ORGBPluginInterface.h          \
    ORGBUpdatePlugin.h             \
    Dependencies/ResourceManager.h \
    Dependencies/RGBController.h   \
    Dependencies/NetworkClient.h   \
    Dependencies/NetworkServer.h   \
    Dependencies/SettingsManager.h \
    Dependencies/NetworkProtocol.h \
    Dependencies/net_port.h        \
    Dependencies/json.hpp          \
    Dependencies/OpenRGBDialog2.h  \

FORMS +=                           \
    ORGBUpdatePlugin.ui            \

RESOURCES +=                       \
    resources.qrc \
    resources.qrc


# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
