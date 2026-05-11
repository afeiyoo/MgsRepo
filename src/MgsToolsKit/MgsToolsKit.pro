include($$PWD/../../Public.pri)

QT       *= core gui network multimedia

TARGET = MgsToolsKit
TEMPLATE = app

win32 {
    DESTDIR = $$MGS_BIN_PATH/win/$$TARGET
} else {
    DESTDIR = $$MGS_BIN_PATH/linux/$$TARGET
}

# 软件版本号
isEmpty(VERSION): VERSION += 0.0.1
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

DEFINES += USE_ELAWIDGETTOOLS
DEFINES += QT_NO_DEBUG_OUTPUT

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include($$THIRD_PARTY_LIBRARY_PATH/utils/Utils.pri)
include($$THIRD_PARTY_LIBRARY_PATH/CuteLogger/CuteLogger.pri)
include($$THIRD_PARTY_LIBRARY_PATH/HttpClient/HttpClient.pri)
include($$THIRD_PARTY_LIBRARY_PATH/NlohmannJson/NlohmannJson.pri)
include($$THIRD_PARTY_LIBRARY_PATH/QZXing/QZXing.pri)
include($$PWD/../SmartLaneController/SmartLaneController.pri)

SOURCES += \
    bend/cardrobot/cardrobothandler.cpp \
    bend/infoboard/infoboardhandler.cpp \
    dialogs/t_cardrobotstatus.cpp \
    global/globalmanager.cpp \
    global/signalmanager.cpp \
    main.cpp \
    mainwindow.cpp \
    pages/t_basepage.cpp \
    pages/t_cardrobot.cpp \
    pages/t_deskprinter.cpp \
    pages/t_infoboard.cpp \
    pages/t_smartcontroller.cpp

HEADERS += \
    bend/cardrobot/cardrobothandler.h \
    bend/infoboard/infoboardhandler.h \
    dialogs/t_cardrobotstatus.h \
    global/appdefs.h \
    global/signalmanager.h \
    mainwindow.h \
    global/constant.h \
    global/globalmanager.h \
    pages/t_basepage.h \
    pages/t_cardrobot.h \
    pages/t_deskprinter.h \
    pages/t_infoboard.h \
    pages/t_smartcontroller.h

# 引入第三方库
unix:!macx|win32: LIBS += \
    -l$$qtLibraryTargetName(ElaWidgetTools)

INCLUDEPATH += \
    $$THIRD_PARTY_LIBRARY_PATH/ElaWidgetTools

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

copyLibsToDestdir($$qtLibraryTargetName(ElaWidgetTools))

RESOURCES += \
    resources.qrc
