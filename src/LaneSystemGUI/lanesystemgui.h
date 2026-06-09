#pragma once

#include "lanesystemgui_global.h"

class QApplication;
class QWidget;

enum class PageType {
    Etc,
    MtcIn,
    MtcOut,
};

namespace LaneSystemGUI {
// 前端初始化
LANESYSTEMGUI_EXPORT void initFront(QApplication &app);

// 创建主窗口
LANESYSTEMGUI_EXPORT QWidget *createMainWindow(PageType page = PageType::Etc, QWidget *parent = nullptr);
} // namespace LaneSystemGUI
