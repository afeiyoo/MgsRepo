include($$PWD/../../Public.pri)

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

# 引入动态库
win32:CONFIG(release, debug|release): LIBS += -L$$MGS_LIBRARY_PATH/win/ -l$$qtLibraryTargetName(LaneSystemGUI)
else:win32:CONFIG(debug, debug|release): LIBS += -L$$MGS_LIBRARY_PATH/win/ -l$$qtLibraryTargetName(LaneSystemGUId)
else:unix:!macx: LIBS += -L$$MGS_LIBRARY_PATH/linux/ -l$$qtLibraryTargetName(LaneSystemGUI)

INCLUDEPATH += $$MGS_INCLUDE_PATH/LaneSystemGUI
DEPENDPATH += $$MGS_INCLUDE_PATH/LaneSystemGUI
