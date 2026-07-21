TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS += \
    # CuteLogger \
    # HttpServer \
    # Jcon \
    # QJson \
    # QSimpleUpdater \
    # LaneMaster \
    # MgsToolsKit \
    # SmartLaneController \
    # LaneDataService \
    # PadService \
    # ETC \
    StationService \
    # ServiceHub

LaneMaster.file = src/LaneMaster/LaneMaster.pro
MgsToolsKit.file = src/MgsToolsKit/MgsToolsKit.pro
SmartLaneController.file = src/SmartLaneController/SmartLaneController.pro
PadService.file = src/PadService/PadService.pro
CuteLogger.file = 3rdparty/CuteLogger/CuteLogger.pro
HttpServer.file = 3rdparty/HttpServer/HttpServer.pro
Jcon.file = 3rdparty/Jcon/Jcon.pro
QJson.file = 3rdparty/QJson/QJson.pro
QSimpleUpdater.file = 3rdparty/QSimpleUpdater/QSimpleUpdater.pro
StationService.file = src/StationService/StationService.pro
ETC.file = src/ETC/ETC.pro
LaneDataService.file = src/LaneDataService/LaneDataService.pro
ServiceHub.file = src/ServiceHub/ServiceHub.pro
