include($$PWD/../../Public.pri)

QT += testlib websockets

QT -= gui

TARGET = Jcon

TEMPLATE = lib
CONFIG += shared dll
TARGET = $$qtLibraryTargetName($$TARGET)

win32 {
    DESTDIR = $$MGS_LIBRARY_PATH/win
} else {
    DESTDIR = $$MGS_LIBRARY_PATH/linux
}

shared|dll {
    DEFINES += JCON_DYNAMIC
}else{
    DEFINES += JCON_STATIC
}

HEADERS = jcon*.h \
    json*.h \
    string_util.h
SOURCES = json*.cpp \
    string_util.cpp
