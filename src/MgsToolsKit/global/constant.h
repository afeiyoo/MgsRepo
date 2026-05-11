#pragma once

#include <QByteArray>

namespace Constant {

namespace APP {
static const QByteArray ICON_PATH = ":/static/images/mgskj_icon.png";
static const QByteArray SOUND_PATH = "/sounds";
} // namespace APP

//日志常量
namespace Log {
static const QByteArray FORMAT = "[%{time}{yyyy-MM-dd HH:mm:ss.zzz}] [%{type}] [%{threadid}] [%{file}:%{line}] | %{message}\n\n";
static const int MAX_SAVE_DAY = 90;
} // namespace Log

namespace CardRobot {
static const QByteArray START = QByteArray::fromHex("FFFF");
static const uchar VER = 0x01;
static const int START_SIZE = 2;
static const int VER_SIZE = 1;
static const int SEQ_SIZE = 1;
static const int LEN_SIZE = 4;
static const int CRC_SIZE = 2;
}; // namespace CardRobot

} // namespace Constant
