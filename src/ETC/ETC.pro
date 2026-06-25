include($$PWD/../../Public.pri)

QT *= core network concurrent
QT -= gui

TARGET = ETC

TEMPLATE = lib
CONFIG += shared dll
TARGET = $$qtLibraryTargetName($$TARGET)

shared|dll {
    DEFINES += ETC_DYNAMIC
} else {
    DEFINES += ETC_STATIC
}

win32 {
    DESTDIR = $$MGS_LIBRARY_PATH/win
} else {
    DESTDIR = $$MGS_LIBRARY_PATH/linux
}

include($$THIRD_PARTY_LIBRARY_PATH/EasyQtSql/EasyQtSql.pri)
include($$THIRD_PARTY_LIBRARY_PATH/utils/Utils.pri)
include($$THIRD_PARTY_LIBRARY_PATH/CuteLogger/CuteLogger.pri)
include($$PWD/../LaneShares/Models/db/DbModels.pri)

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    bend/bizhandler.cpp \
    bend/devshandler.cpp \
    bend/environment.cpp \
    dao/configs/config.cpp \
    dao/configs/configini.cpp \
    dao/configs/configjson.cpp \
    dao/dbs/dataservice.cpp \
    dao/dbs/dataservicedameng.cpp \
    dao/dbs/dataservicemysql.cpp \
    etc.cpp \
    fend/pagehandler.cpp \
    global/baseexception.cpp \
    middle/gateway.cpp \
    global/globalmanager.cpp \
    middle/signalctrl.cpp

HEADERS += \
    bend/bizhandler.h \
    bend/devshandler.h \
    bend/environment.h \
    dao/configs/config.h \
    dao/configs/configini.h \
    dao/configs/configjson.h \
    dao/dbs/dataservice.h \
    dao/dbs/dataservicedameng.h \
    dao/dbs/dataservicemysql.h \
    etc.h \
    etc_global.h \
    fend/pagehandler.h \
    global/apis.h \
    global/baseexception.h \
    global/const.h \
    global/errcode.h \
    global/etcdefs.h \
    middle/gateway.h \
    global/globalmanager.h \
    middle/signalctrl.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += \
    $$MGS_INCLUDE_PATH/LaneSystemGUI
DEPENDPATH += \
    $$MGS_INCLUDE_PATH/LaneSystemGUI

# 引用头文件导出
PUBLIC_HEADERS = \
    $$PWD/etc.h \
    $$PWD/etc_global.h
copyHeadersToInclude($$TARGET, PUBLIC_HEADERS)

RESOURCES += \
    resource.qrc

