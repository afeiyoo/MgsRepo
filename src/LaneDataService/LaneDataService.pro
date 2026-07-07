include($$PWD/../../Public.pri)E

QT -= gui
QT += network sql

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
    config/config.cpp \
    core/globalmanager.cpp \
    dbs/dataservice.cpp \
    dbs/dataservicemysql.cpp \
    lanedataservice.cpp

HEADERS += \
    LaneDataService_global.h \
    config/config.h \
    core/globalmanager.h \
    dbs/dataservice.h \
    dbs/dataservicemysql.h \
    lanedataservice.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
