include($$PWD/../../Public.pri)

QT -= gui
QT += network sql xml concurrent

TARGET = LaneDataService

TEMPLATE = lib
CONFIG += shared dll
TARGET = $$qtLibraryTargetName($$TARGET)

win32 {
    DESTDIR = $$MGS_LIBRARY_PATH/win
} else {
    DESTDIR = $$MGS_LIBRARY_PATH/linux
}

shared|dll {
    DEFINES += LANEDATASERVICE_DYNAMIC
} else {
    DEFINES += LANEDATASERVICE_STATIC
}

include($$THIRD_PARTY_LIBRARY_PATH/CuteLogger/CuteLogger.pri)
include($$THIRD_PARTY_LIBRARY_PATH/utils/Utils.pri)
include($$THIRD_PARTY_LIBRARY_PATH/EasyQtSql/EasyQtSql.pri)


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    bend/fullblackchecker.cpp \
    config/config.cpp \
    core/baseexception.cpp \
    core/gateway.cpp \
    core/globalmanager.cpp \
    core/signalmanager.cpp \
    dbs/dataservice.cpp \
    dbs/sqldealer.cpp \
    lanedataservice.cpp

HEADERS += \
    bend/fullblackchecker.h \
    config/config.h \
    core/baseexception.h \
    core/gateway.h \
    core/globalmanager.h \
    core/signalmanager.h \
    dbs/dataservice.h \
    dbs/sqldealer.h \
    ilanedataservice.h \
    lanedataservice.h \
    lanedataservice_global.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += \
    $$MGS_INCLUDE_PATH/ServiceHub

PUBLIC_HEADERS = \
    $$PWD/ilanedataservice.h \
    $$PWD/lanedataservice_global.h
copyHeadersToInclude(LaneDataService, PUBLIC_HEADERS)

RESOURCES += \
    resources.qrc
