include($$PWD/../../Public.pri)

QT -= gui

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

# 引入QXlsx库
QXLSX_PARENTPATH = $$THIRD_PARTY_LIBRARY_PATH/QXlsx
QXLSX_HEADERPATH = $$THIRD_PARTY_LIBRARY_PATH/QXlsx/header
QXLSX_SOURCEPATH = $$THIRD_PARTY_LIBRARY_PATH/QXlsx/source
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

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
