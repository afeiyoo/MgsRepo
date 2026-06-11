QT *= xml sql

HEADERS += \
    $$PWD/algorithm.h \
    $$PWD/bizutils.h \
    $$PWD/configutils.h \
    $$PWD/datadealutils.h \
    $$PWD/defs.h \
    $$PWD/enums.h \
    $$PWD/fileutils.h \
    $$PWD/hostosinfo.h \
    $$PWD/httplib.h \
    $$PWD/networkutils.h \
    $$PWD/optional.h \
    $$PWD/osspecificaspects.h \
    $$PWD/predicates.h \
    $$PWD/qtcassert.h \
    $$PWD/savefile.h \
    $$PWD/sm4.h \
    $$PWD/sqlutils.h \
    $$PWD/stdafx.h \
    $$PWD/iniutils.h

SOURCES += \
    $$PWD/bizutils.cpp \
    $$PWD/configutils.cpp \
    $$PWD/datadealutils.cpp \
    $$PWD/fileutils.cpp \
    $$PWD/hostosinfo.cpp \
    $$PWD/networkutils.cpp \
    $$PWD/qtcassert.cpp \
    $$PWD/savefile.cpp \
    $$PWD/sm4.cpp \
    $$PWD/sqlutils.cpp \
    $$PWD/iniutils.cpp

contains(QT, gui) {
    SOURCES += $$PWD/uiutils.cpp
    HEADERS += $$PWD/uiutils.h
}

INCLUDEPATH += \
    $$PWD \
    $$PWD/../

DEPENDPATH += \
    $$PWD \
    $$PWD/../
