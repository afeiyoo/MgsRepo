include($$PWD/../../Public.pri)

QT -= gui

TARGET = HttpServer

TEMPLATE = lib
CONFIG += shared dll
TARGET = $$qtLibraryTargetName($$TARGET)

win32 {
    DESTDIR = $$MGS_LIBRARY_PATH/win
} else {
    DESTDIR = $$MGS_LIBRARY_PATH/linux
}

win32 {
   DEFINES += QTWEBAPPLIB_EXPORT
}

include($$PWD/HttpServer.pri)
