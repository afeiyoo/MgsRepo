include($$PWD/../../Public.pri)

QT *= core gui network

TARGET = LaneSystemGUI
TEMPLATE = app

win32 {
    DESTDIR = $$MGS_BIN_PATH/win
} else {
    DESTDIR = $$MGS_BIN_PATH/linux
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
    dialogs/authdialog.cpp \
    global/baseexception.cpp \
    global/globalmanager.cpp \
    global/laneui.cpp \
    global/signalmanager.cpp \
    lanemaster.cpp \
    main.cpp \
    pages/basepage.cpp \
    pages/etcpage.cpp \
    pages/mainwindow.cpp \
    pages/mtcinpage.cpp \
    pages/mtcoutpage.cpp

HEADERS += \
    IPageController.h \
    components/devicepanel.h \
    components/iconbutton.h \
    components/pagearea.h \
    components/recenttradepanel.h \
    components/scrolltext.h \
    components/weightinfopanel.h \
    delegate/weightinfodelegate.h \
    dialogs/authdialog.h \
    global/baseexception.h \
    global/globalmanager.h \
    global/laneui.h \
    global/signalmanager.h \
    global/uiconst.h \
    lanemaster.h \
    pages/basepage.h \
    pages/etcpage.h \
    pages/mainwindow.h \
    pages/mtcinpage.h \
    pages/mtcoutpage.h

# 引入第三方库
unix:!macx|win32: LIBS += \
    -l$$qtLibraryTargetName(ElaWidgetTools) -l$$qtLibraryTargetName(ServiceHub) \
    -l$$qtLibraryTargetName(LaneDataService)

INCLUDEPATH += \
    $$THIRD_PARTY_LIBRARY_PATH/ElaWidgetTools \
    $$MGS_INCLUDE_PATH/ServiceHub \
    $$MGS_INCLUDE_PATH/LaneDataService

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
