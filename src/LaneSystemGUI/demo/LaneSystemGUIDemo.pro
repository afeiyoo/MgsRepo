include($$PWD/../../../Public.pri)

QT *= core gui network

TARGET = LaneSystemGUIDemo
TEMPLATE = app

win32 {
    DESTDIR = $$MGS_BIN_PATH/win/$$TARGET
} else {
    DESTDIR = $$MGS_BIN_PATH/linux/$$TARGET
}

include($$THIRD_PARTY_LIBRARY_PATH/utils/Utils.pri)
include($$THIRD_PARTY_LIBRARY_PATH/CuteLogger/CuteLogger.pri)

SOURCES += \
    main.cpp

INCLUDEPATH += \
    $$PWD/.. \
    $$PWD/../components \
    $$PWD/../delegate \
    $$PWD/../global \
    $$PWD/../pages

win32: LIBS += \
    -L$$MGS_LIBRARY_PATH/win \
    -l$$qtLibraryTargetName(LaneSystemGUI)

unix:!macx {
    LIBS += \
        -L$$MGS_LIBRARY_PATH/linux \
        -l$$qtLibraryTargetName(LaneSystemGUI)
}
