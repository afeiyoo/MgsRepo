include($$PWD/../../Public.pri)

QT       *= core gui network multimedia

TARGET = MgsToolsKit
TEMPLATE = app

# 软件版本号
isEmpty(VERSION): VERSION += 0.0.1
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

DEFINES += USE_ELAWIDGETTOOLS
DEFINES += QT_NO_DEBUG_OUTPUT

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include($$THIRD_PARTY_LIBRARY_PATH/utils/Utils.pri)
include($$THIRD_PARTY_LIBRARY_PATH/HttpClient/HttpClient.pri)
include($$THIRD_PARTY_LIBRARY_PATH/NlohmannJson/NlohmannJson.pri)
include($$THIRD_PARTY_LIBRARY_PATH/QZXing/QZXing.pri)
include($$PWD/../SmartLaneController/SmartLaneController.pri)

SOURCES += \
    bend/cardrobot/cardrobothandler.cpp \
    bend/infoboard/infoboardhandler.cpp \
    dialogs/t_cardrobotstatus.cpp \
    global/globalmanager.cpp \
    global/signalmanager.cpp \
    main.cpp \
    mainwindow.cpp \
    pages/t_basepage.cpp \
    pages/t_cardrobot.cpp \
    pages/t_deskprinter.cpp \
    pages/t_infoboard.cpp \
    pages/t_mobileplusterminal.cpp \
    pages/t_smartcontroller.cpp

HEADERS += \
    bend/cardrobot/cardrobothandler.h \
    bend/infoboard/infoboardhandler.h \
    dialogs/t_cardrobotstatus.h \
    global/appdefs.h \
    global/signalmanager.h \
    mainwindow.h \
    global/constant.h \
    global/globalmanager.h \
    pages/t_basepage.h \
    pages/t_cardrobot.h \
    pages/t_deskprinter.h \
    pages/t_infoboard.h \
    pages/t_mobileplusterminal.h \
    pages/t_smartcontroller.h

# 引入第三方库
unix:!macx|win32: LIBS += \
    -l$$qtLibraryTargetName(ElaWidgetTools) \
    -l$$qtLibraryTargetName(CuteLogger) \
    -l$$qtLibraryTargetName(MobilePlusTerminal)

INCLUDEPATH += \
    $$MGS_INCLUDE_PATH/ElaWidgetTools \
    $$MGS_INCLUDE_PATH/CuteLogger \
    $$MGS_INCLUDE_PATH/MobilePlusTerminal

# 交付安装
win32 {
    INSTALL_DIR = $$MGS_BIN_PATH/win/$$TARGET
} else {
    INSTALL_DIR = $$MGS_BIN_PATH/linux/$$TARGET
}

target.path = $$INSTALL_DIR

win32 {
    RUNTIME_LIBRARIES = \
        $$MGS_LIBRARY_PATH/win/$${qtLibraryTargetName(ElaWidgetTools)}.dll \
        $$MGS_LIBRARY_PATH/win/$${qtLibraryTargetName(CuteLogger)}.dll \
        $$MGS_LIBRARY_PATH/win/$${qtLibraryTargetName(MobilePlusTerminal)}.dll
} else {
    RUNTIME_LIBRARIES = \
        $$MGS_LIBRARY_PATH/linux/lib$${qtLibraryTargetName(ElaWidgetTools)}.so* \
        $$MGS_LIBRARY_PATH/linux/lib$${qtLibraryTargetName(CuteLogger)}.so* \
        $$MGS_LIBRARY_PATH/linux/lib$${qtLibraryTargetName(MobilePlusTerminal)}.so*

    # 动态库和可执行程序安装在同一个目录
    QMAKE_LFLAGS += -Wl,-rpath=\'\$$ORIGIN\'
}

runtime_libraries.files = $$RUNTIME_LIBRARIES
runtime_libraries.path = $$INSTALL_DIR
runtime_libraries.CONFIG += no_check_exist

INSTALLS += target runtime_libraries

RESOURCES += \
    resources.qrc
