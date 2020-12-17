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

SOURCES +=                                      \
    ORGBUpdatePlugin.cpp                        \
    Dependencies/ResourceManager.cpp            \
    Dependencies/RGBController.cpp              \
    Dependencies/RGBController_Network.cpp      \
    Dependencies/NetworkClient.cpp              \
    Dependencies/NetworkServer.cpp              \
    Dependencies/NetworkProtocol.cpp            \
    Dependencies/SettingsManager.cpp            \
    Dependencies/net_port.cpp                   \

HEADERS +=                                      \
    ORGBPluginInterface.h                       \
    ORGBUpdatePlugin.h                          \
    Dependencies/ResourceManager.h              \
    Dependencies/RGBController.h                \
    Dependencies/RGBController_Network.h        \
    Dependencies/NetworkServer.h                \
    Dependencies/NetworkProtocol.h              \
    Dependencies/SettingsManager.h              \
    Dependencies/net_port.h                     \
    Dependencies/json.hpp                       \

FORMS +=                                        \
    ORGBUpdatePlugin.ui                         \

RESOURCES +=                                    \
    resources.qrc                               \


# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
