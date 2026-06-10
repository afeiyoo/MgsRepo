#pragma once

//日志常量
namespace Log {
const char FORMAT[] = "[%{time}{yyyy-MM-dd HH:mm:ss.zzz}] [%{type}] [%{threadid}] | %{message}\n\n";
const int MAX_SAVE_DAY = 90;
} // namespace Log

namespace Path {
const char DISPLAY_FONT[] = ":/static/font/HarmonyOS_Sans_SC_Regular.ttf";
const char APP_BIG_ICON[] = ":/static/images/banner.png";
const char CAP_AREA_BACKGROUND[] = ":/static/images/cap_background.png";
const char WHITE_PLATE[] = ":/static/images/white_plate.png";
const char BLACK_PLATE[] = ":/static/images/black_plate.png";
const char YELLOW_PLATE[] = ":/static/images/yellow_plate.png";
const char GREEN_PLATE[] = ":/static/images/green_plate.png";
const char GRADIENT_PLATE[] = ":/static/images/gradient_plate.png";
const char MIX_PLATE[] = ":/static/images/mix_plate.png";
const char BLUE_PLATE[] = ":/static/images/blue_plate.png";
const char TEMP_PLATE[] = ":/static/images/temp_plate.png";
} // namespace Path

namespace Color {
const char TOP_WIDGET_BG[] = "#EFF1F6";      // 顶栏背景色
const char MAIN_BG[] = "#D0D6E3";            // 主界面背景色
const char CUSTOM_AREA_BG[] = "#EFF1F6";     // 区域背景色
const char INSIDE_AREA_BG[] = "#EBEDF3";     // 内部信息区域背景色
const char CONFIRM_BUTTON_BG[] = "#007BFF";  // 确认按钮背景色
const char WARN_BUTTON_BG[] = "#FC3C3C";     // 警告背景色
const char INFO_BUTTON_BG[] = "#00BB65";     // 提示背景色
const char WEIGHTINFO_ITEM_BG[] = "#FFFFFF"; // 称重信息背景色

const char STATUS_TC[] = "#00BB65"; // 状态字体色
const char INFO_TC[] = "#0C4E94";   // 提示字体色
const char BUTTON_TC[] = "#FFFFFF"; // 按钮字体色
const char WARN_TC[] = "#FC3C3C";   // 警告字体色
} // namespace Color
