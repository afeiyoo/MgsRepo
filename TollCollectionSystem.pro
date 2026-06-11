TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS += \
    # CuteLogger \
    # HttpServer \
    # Jcon \
    # QJson \
    # QSimpleUpdater \
    LaneSystemGUI \
    LaneSystemGUIDemo \
    # MgsToolsKit \
    # SmartLaneController \
    # PadService \

LaneSystemGUI.file = src/LaneSystemGUI/LaneSystemGUI.pro
LaneSystemGUIDemo.file = src/LaneSystemGUI/demo/LaneSystemGUIDemo.pro
MgsToolsKit.file = src/MgsToolsKit/MgsToolsKit.pro
SmartLaneController.file = src/SmartLaneController/SmartLaneController.pro
PadService.file = src/PadService/PadService.pro
CuteLogger.file = 3rdparty/CuteLogger/CuteLogger.pro
HttpServer.file = 3rdparty/HttpServer/HttpServer.pro
Jcon.file = 3rdparty/Jcon/Jcon.pro
QJson.file = 3rdparty/QJson/QJson.pro
QSimpleUpdater.file = 3rdparty/QSimpleUpdater/QSimpleUpdater.pro

LaneSystemGUIDemo.depends = LaneSystemGUI
