TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS += \
    # CuteLogger \
    # HttpServer \
    # Jcon \
    # QJson \
    # QSimpleUpdater \
    # LaneSystemGUI \
    # LaneSystemGUIDemo \
    # MgsToolsKit \
    # SmartLaneController \
    # PadService \
    LaneSystemMaster \
    ETC

LaneSystemGUI.file = src/LaneSystemGUI/LaneSystemGUI.pro
LaneSystemGUIDemo.file = src/LaneSystemGUIDemo/LaneSystemGUIDemo.pro
MgsToolsKit.file = src/MgsToolsKit/MgsToolsKit.pro
SmartLaneController.file = src/SmartLaneController/SmartLaneController.pro
PadService.file = src/PadService/PadService.pro
CuteLogger.file = 3rdparty/CuteLogger/CuteLogger.pro
HttpServer.file = 3rdparty/HttpServer/HttpServer.pro
Jcon.file = 3rdparty/Jcon/Jcon.pro
QJson.file = 3rdparty/QJson/QJson.pro
QSimpleUpdater.file = 3rdparty/QSimpleUpdater/QSimpleUpdater.pro
LaneSystemMaster.file = src/LaneSystemMaster/LaneSystemMaster.pro
ETC.file = src/ETC/ETC.pro

LaneSystemGUIDemo.depends = LaneSystemGUI
ETC.depends = LaneSystemGUI
