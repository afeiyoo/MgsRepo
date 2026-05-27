include($$PWD/../../Public.pri)

QT *= core gui network

TARGET = LaneSystemGUI

TEMPLATE = app

DEFINES += USE_ELAWIDGETTOOLS

contains(TEMPLATE, lib) {
    message("Build Dynamic Library")

    CONFIG += shared dll
    TARGET = $$qtLibraryTarget($$TARGET)
    DEFINES += LANESYSTEMGUI_DYNAMIC

    win32 {
        DESTDIR = $$MGS_LIBRARY_PATH/win
    } else {
        DESTDIR = $$MGS_LIBRARY_PATH/linux
    }
} else {
    message("Build Application")

    DEFINES += LANESYSTEMGUI_STATIC

    win32 {
        DESTDIR = $$MGS_BIN_PATH/win/$$TARGET
    } else {
        DESTDIR = $$MGS_BIN_PATH/linux/$$TARGET
    }
}

include($$THIRD_PARTY_LIBRARY_PATH/utils/Utils.pri)
include($$THIRD_PARTY_LIBRARY_PATH/CuteLogger/CuteLogger.pri)

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    components/devicepanel.cpp \
    components/iconbutton.cpp \
    components/pagearea.cpp \
    components/recenttradepanel.cpp \
    components/scrolltext.cpp \
    components/weightinfopanel.cpp \
    delegate/weightinfodelegate.cpp \
    global/globalmanager.cpp \
    global/signalmanager.cpp \
    pages/basepage.cpp \
    pages/mainwindow.cpp \
    main.cpp \
    pages/mtcinpage.cpp

HEADERS += \
    components/devicepanel.h \
    components/iconbutton.h \
    components/pagearea.h \
    components/recenttradepanel.h \
    components/scrolltext.h \
    components/weightinfopanel.h \
    delegate/weightinfodelegate.h \
    global/globalmanager.h \
    global/signalmanager.h \
    global/uiconst.h \
    global/uidefs.h \
    pages/basepage.h \
    pages/mainwindow.h \
    pages/mtcinpage.h
    lanesystemgui_global.h \

# 引入第三方库
unix:!macx|win32: LIBS += \
    -l$$qtLibraryTargetName(ElaWidgetTools)

INCLUDEPATH += \
    $$THIRD_PARTY_LIBRARY_PATH/ElaWidgetTools

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
