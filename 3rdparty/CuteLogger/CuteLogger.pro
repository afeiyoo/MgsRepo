include($$PWD/../../Public.pri)

QT -= gui

TARGET = CuteLogger

TEMPLATE = lib
CONFIG += shared dll
TARGET = $$qtLibraryTargetName($$TARGET)

win32 {
    DESTDIR = $$MGS_LIBRARY_PATH/win
} else {
    DESTDIR = $$MGS_LIBRARY_PATH/linux
}

include($$PWD/CuteLogger.pri)

# 交付安装
PUBLIC_HEADERS = \
    $$PWD/include/Logger.h \
    $$PWD/include/CuteLogger_global.h \
    $$PWD/include/AbstractAppender.h \
    $$PWD/include/AbstractStringAppender.h \
    $$PWD/include/ConsoleAppender.h \
    $$PWD/include/FileAppender.h \
    $$PWD/include/RollingFileAppender.h

win32 {
    target.path = $$MGS_BIN_PATH/win/$$TARGET/lib
    public_headers.path = $$MGS_BIN_PATH/win/$$TARGET/include
} else {
    target.path = $$MGS_BIN_PATH/linux/$$TARGET/lib
    public_headers.path = $$MGS_BIN_PATH/linux/$$TARGET/include
}

public_headers.files = $$PUBLIC_HEADERS

INSTALLS += target public_headers
