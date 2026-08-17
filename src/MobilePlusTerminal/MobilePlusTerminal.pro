include($$PWD/../../Public.pri)

QT -= gui
QT += network

TARGET = MobilePlusTerminal

TEMPLATE = lib
CONFIG += shared dll
TARGET = $$qtLibraryTargetName($$TARGET)

win32 {
    DESTDIR = $$MGS_LIBRARY_PATH/win
} else {
    DESTDIR = $$MGS_LIBRARY_PATH/linux
}

shared|dll {
    DEFINES += MOBILEPLUSTERMINAL_DYNAMIC
} else {
    DEFINES += MOBILEPLUSTERMINAL_STATIC
}

include($$THIRD_PARTY_LIBRARY_PATH/utils/Utils.pri)

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cmdhandler.cpp \
    mobileplusterminal.cpp

HEADERS += \
    cmdhandler.h \
    defines.h \
    imobileplusterminal.h \
    mobileplusterminal.h \
    mobileplusterminal_global.h

# 引入第三方库
unix:!macx|win32: LIBS += \
    -l$$qtLibraryTargetName(CuteLogger)

INCLUDEPATH += \
    $$MGS_INCLUDE_PATH/CuteLogger

# 交付安装
PUBLIC_HEADERS = \
    $$PWD/mobileplusterminal_global.h \
    $$PWD/imobileplusterminal.h

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
