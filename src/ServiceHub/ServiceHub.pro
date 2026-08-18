include($$PWD/../../Public.pri)

QT -= gui

TARGET = ServiceHub

TEMPLATE = lib
CONFIG += shared dll
TARGET = $$qtLibraryTargetName($$TARGET)

win32 {
    DESTDIR = $$MGS_LIBRARY_PATH/win
} else {
    DESTDIR = $$MGS_LIBRARY_PATH/linux
}

shared|dll {
    DEFINES += SERVICEHUB_DYNAMIC
} else {
    DEFINES += SERVICEHUB_STATIC
}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    servicehub.cpp

HEADERS += \
    iservicehub.h \
    servicehub.h \
    servicehub_global.h

PUBLIC_HEADERS = \
    $$PWD/servicehub_global.h \
    $$PWD/iservicehub.h
copyHeadersToInclude(ServiceHub, PUBLIC_HEADERS)

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
