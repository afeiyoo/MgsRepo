include($$PWD/../../Public.pri)

QT *= core network
QT -= gui

CONFIG += cmdline

TARGET = CardService
TEMPLATE = app

win32 {
    DESTDIR = $$MGS_BIN_PATH/win/$$TARGET
} else {
    DESTDIR = $$MGS_BIN_PATH/linux/$$TARGET
}

include($$THIRD_PARTY_LIBRARY_PATH/utils/Utils.pri)
CONFIG += qtservice-buildlib
include($$THIRD_PARTY_LIBRARY_PATH/qtservice/src/qtservice.pri)
include($$THIRD_PARTY_LIBRARY_PATH/HttpServer/HttpServer.pri)
include($$THIRD_PARTY_LIBRARY_PATH/CuteLogger/CuteLogger.pri)

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        bend/authpsamhandler.cpp \
        bend/closereaderhandler.cpp \
        bend/readcardhandler.cpp \
        bend/readpsamhandler.cpp \
        bend/setconfighandler.cpp \
        bend/verifycardhandler.cpp \
        config/config.cpp \
        core/cardserviceapp.cpp \
        core/globalmanager.cpp \
        core/readerctrl.cpp \
        core/requestmapper.cpp \
        helper/CCardTransform.cpp \
        helper/CHexBufTool.cpp \
        helper/CommonTool.cpp \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    bend/authpsamhandler.h \
    bend/closereaderhandler.h \
    bend/readcardhandler.h \
    bend/readpsamhandler.h \
    bend/setconfighandler.h \
    bend/verifycardhandler.h \
    config/config.h \
    core/cardserviceapp.h \
    core/globalmanager.h \
    core/readerctrl.h \
    core/requestmapper.h \
    def/defines.h \
    helper/CCardTransform.h \
    helper/CHexBufTool.h \
    helper/CommonTool.h \
    helper/ILaneCardCtrl.h


unix:!macx|win32: LIBS += \
    -l$$qtLibraryTargetName(QXCardCtrl)

INCLUDEPATH += \
    $$THIRD_PARTY_LIBRARY_PATH/mgslibs
