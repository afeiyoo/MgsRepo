include($$PWD/../../Public.pri)

QT *= core network sql
QT -= gui

CONFIG += cmdline

TARGET = PadService
TEMPLATE = app

win32 {
    DESTDIR = $$MGS_BIN_PATH/win/$$TARGET
} else {
    DESTDIR = $$MGS_BIN_PATH/linux/$$TARGET
}

include($$THIRD_PARTY_LIBRARY_PATH/EasyQtSql/EasyQtSql.pri)
include($$THIRD_PARTY_LIBRARY_PATH/HttpClient/HttpClient.pri)
include($$THIRD_PARTY_LIBRARY_PATH/CuteLogger/CuteLogger.pri)
include($$THIRD_PARTY_LIBRARY_PATH/utils/Utils.pri)
include($$THIRD_PARTY_LIBRARY_PATH/HttpServer/HttpServer.pri)
include($$THIRD_PARTY_LIBRARY_PATH/NlohmannJson/NlohmannJson.pri)

SOURCES += \
    bend/bizhandler.cpp \
    dataservice/dataservice.cpp \
    config/config.cpp \
    core/baseexception.cpp \
    core/dtpsender.cpp \
    core/httphandler.cpp \
    core/globalmanager.cpp \
    main.cpp

HEADERS += \
    bean/t_auditpayback.h \
    bean/t_discardticketreview.h \
    bean/t_etcout.h \
    bean/t_freetempvehicle.h \
    bean/t_laneinputshift.h \
    bean/t_mtcout.h \
    bean/t_specialcards.h \
    bean/t_splitout.h \
    bean/t_mticketuse.h \
    bend/bizhandler.h \
    dataservice/dataservice.h \
    config/config.h \
    global/defs.h \
    core/baseexception.h \
    core/dtpsender.h \
    core/httphandler.h \
    core/globalmanager.h
