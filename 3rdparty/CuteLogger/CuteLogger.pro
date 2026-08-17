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
    INSTALL_DIR = $$MGS_BIN_PATH/win/$$TARGET
} else {
    INSTALL_DIR = $$MGS_BIN_PATH/linux/$$TARGET
}

target.path = $$INSTALL_DIR/lib

target_headers.files = $$PUBLIC_HEADERS
target_headers.path = $$INSTALL_DIR/include

public_include.files = $$PUBLIC_HEADERS
public_include.path = $$MGS_INCLUDE_PATH/$$TARGET

INSTALLS += target target_headers public_include
