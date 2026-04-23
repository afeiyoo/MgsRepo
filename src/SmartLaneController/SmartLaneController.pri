shared|dll {
    DEFINES += SMARTLANECONTROLLER_DYNAMIC
} else {
    DEFINES += SMARTLANECONTROLLER_STATIC
}

DEFINES += SMARTLANECONTROLLER_NO_CREATE_LOG    # 由外部统一管理日志

SOURCES += \
    $$PWD/smartlanecontroller.cpp

HEADERS += \
    $$PWD/smartlanecontroller.h \
    $$PWD/smartlanecontroller_global.h

INCLUDEPATH += $$PWD
