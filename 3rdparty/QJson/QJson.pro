include($$PWD/../../Public.pri)

QT -= gui

TARGET = QJson

TEMPLATE = lib
CONFIG += shared dll
TARGET = $$qtLibraryTargetName($$TARGET)

win32 {
    DESTDIR = $$MGS_LIBRARY_PATH/win
} else {
    DESTDIR = $$MGS_LIBRARY_PATH/linux
}

shared|dll {
    DEFINES += QJSON_DYNAMIC
}else{
    DEFINES += QJSON_STATIC
}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    $$PWD/include/json_parser.hh \
    $$PWD/include/json_scanner.h \
    $$PWD/include/location.hh \
    $$PWD/include/parser_p.h \
    $$PWD/include/position.hh \
    $$PWD/include/qjson_debug.h \
    $$PWD/include/stack.hh \
    $$PWD/include/parser.h \
    $$PWD/include/parserrunnable.h \
    $$PWD/include/qobjecthelper.h \
    $$PWD/include/serializer.h \
    $$PWD/include/serializerrunnable.h \
    $$PWD/include/qjson_export.h

SOURCES += \
    $$PWD/src/json_parser.cc \
    $$PWD/src/json_scanner.cpp \
    $$PWD/src/parser.cpp \
    $$PWD/src/parserrunnable.cpp \
    $$PWD/src/qobjecthelper.cpp \
    $$PWD/src/serializer.cpp \
    $$PWD/src/serializerrunnable.cpp \
    $$PWD/src/json_scanner.cc

INCLUDEPATH += $$PWD/include

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
