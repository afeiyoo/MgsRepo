include($$PWD/../../Public.pri)

QT *= core gui

TARGET = LaneSystemMaster
TEMPLATE = app

win32 {
    DESTDIR = $$MGS_BIN_PATH/win/$$TARGET
} else {
    DESTDIR = $$MGS_BIN_PATH/linux/$$TARGET
}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    lanesystemmaster.cpp \
    main.cpp

HEADERS += \
    lanesystemmaster.h

include($$PWD/../LaneShares/Models/Models.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# 引入动态库
win32:CONFIG(release, debug|release): LIBS += -L$$MGS_LIBRARY_PATH/win/ -l$$qtLibraryTargetName(LaneSystemGUI) -l$$qtLibraryTargetName(ETC)
else:win32:CONFIG(debug, debug|release): LIBS += -L$$MGS_LIBRARY_PATH/win/ -l$$qtLibraryTargetName(LaneSystemGUId) -l$$qtLibraryTargetName(ETCd)
else:unix:!macx: LIBS += -L$$MGS_LIBRARY_PATH/linux/ -l$$qtLibraryTargetName(LaneSystemGUI) -l$$qtLibraryTargetName(ETC)

INCLUDEPATH += \
    $$MGS_INCLUDE_PATH/LaneSystemGUI \
    $$MGS_INCLUDE_PATH/ETC
DEPENDPATH += \
    $$MGS_INCLUDE_PATH/LaneSystemGUI \
    $$MGS_INCLUDE_PATH/ETC
