include($$PWD/../../Public.pri)

QT -= gui
QT += network

TARGET = SmartLaneController

TEMPLATE = lib
CONFIG += shared dll
TARGET = $$qtLibraryTargetName($$TARGET)

win32 {
    DESTDIR = $$MGS_LIBRARY_PATH/win
} else {
    DESTDIR = $$MGS_LIBRARY_PATH/linux
}

shared|dll {
    DEFINES += SMARTLANECONTROLLER_DYNAMIC
} else {
    DEFINES += SMARTLANECONTROLLER_STATIC
}

include($$THIRD_PARTY_LIBRARY_PATH/CuteLogger/CuteLogger.pri)
include($$THIRD_PARTY_LIBRARY_PATH/utils/Utils.pri)

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    smartlanecontroller.cpp

HEADERS += \
    smartlanecontroller.h \
    smartlanecontroller_global.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
