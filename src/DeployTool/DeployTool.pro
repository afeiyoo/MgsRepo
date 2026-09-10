include($$PWD/../../Public.pri)

QT -= gui
QT += network sql

TARGET = DeployTool

TEMPLATE = lib
CONFIG += shared dll
TARGET = $$qtLibraryTargetName($$TARGET)

win32 {
    DESTDIR = $$MGS_LIBRARY_PATH/win
} else {
    DESTDIR = $$MGS_LIBRARY_PATH/linux
}

shared|dll {
    DEFINES += DEPLOYTOOL_DYNAMIC
} else {
    DEFINES += DEPLOYTOOL_STATIC
}

include($$THIRD_PARTY_LIBRARY_PATH/utils/Utils.pri)
include($$THIRD_PARTY_LIBRARY_PATH/EasyQtSql/EasyQtSql.pri)

# 引入QXlsx库
QXLSX_PARENTPATH = $$THIRD_PARTY_LIBRARY_PATH/QXlsx/
QXLSX_HEADERPATH = $$THIRD_PARTY_LIBRARY_PATH/QXlsx/header/
QXLSX_SOURCEPATH = $$THIRD_PARTY_LIBRARY_PATH/QXlsx/source/
include($$THIRD_PARTY_LIBRARY_PATH/QXlsx/QXlsx.pri)

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    deploytool.cpp

HEADERS += \
    define.h \
    deploytool.h \
    deploytool_global.h \
    ideploytool.h

# 引入第三方库
unix:!macx|win32: LIBS += \
    -l$$qtLibraryTargetName(CuteLogger)

INCLUDEPATH += \
    $$MGS_INCLUDE_PATH/CuteLogger

DISTFILES += \
    $$MGS_SCRIPT_PATH/update_network.sh

# 交付安装
PUBLIC_HEADERS = \
    $$PWD/deploytool_global.h \
    $$PWD/ideploytool.h

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
