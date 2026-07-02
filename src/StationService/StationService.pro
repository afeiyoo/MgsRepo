include($$PWD/../../Public.pri)

QT *= core network sql
QT -= gui

CONFIG += cmdline

TARGET = StationService
TEMPLATE = app

win32 {
    DESTDIR = $$MGS_BIN_PATH/win/$$TARGET
} else {
    DESTDIR = $$MGS_BIN_PATH/linux/$$TARGET
}

include($$THIRD_PARTY_LIBRARY_PATH/utils/Utils.pri)
include($$THIRD_PARTY_LIBRARY_PATH/HttpServer/HttpServer.pri)
include($$THIRD_PARTY_LIBRARY_PATH/CuteLogger/CuteLogger.pri)
include($$THIRD_PARTY_LIBRARY_PATH/HttpClient/HttpClient.pri)
include($$THIRD_PARTY_LIBRARY_PATH/EasyQtSql/EasyQtSql.pri)

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        bend/cron.cpp \
        bend/mainhandler.cpp \
        core/baseexception.cpp \
        core/httphandler.cpp \
        config/config.cpp \
        core/globalmanager.cpp \
        dbs/dataservice.cpp \
        dbs/dataservicedameng.cpp \
        dbs/dataservicesqlserver.cpp \
        main.cpp

HEADERS += \
    bend/cron.h \
    bend/mainhandler.h \
    core/baseexception.h \
    core/httphandler.h \
    config/config.h \
    core/globalmanager.h \
    dbs/dataservice.h \
    dbs/dataservicedameng.h \
    dbs/dataservicesqlserver.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


