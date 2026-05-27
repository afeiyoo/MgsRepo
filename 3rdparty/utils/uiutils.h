#pragma once

#include <QBoxLayout>
#include <QColor>
#include <QMessageBox>
#include <QObject>
#include <QProxyStyle>
#include <QRegularExpression>

namespace Utils {

class FileName;
class UiUtils : public QObject
{
    Q_OBJECT
public:
    explicit UiUtils(QObject *parent = nullptr);
    ~UiUtils() override;

    // 添加分割线
    static void addLine(QBoxLayout *layout, int thickness = 3, int height = 10, QString color = "#cdcdcd");

    // 为控件widget应用阴影效果
    static void applyShadow(QWidget *widget);

#if defined(USE_ELAWIDGETTOOLS)
    // 返回提示控件
    static QHBoxLayout *createTipWidget(const QString &tip);
#endif

    // 将控件移动到屏幕中央
    static void moveToCenter(QWidget *widget);

    // 屏蔽鼠标事件
    static void disableMouseEvents(QWidget *widget);

    // 截取当前主屏幕并保存到指定文件夹
    static bool screenShot(FileName filePath, QString *error = nullptr);

    // 显示提示对话框
    static QMessageBox::StandardButton showMessageBoxInfo(const QString &title, const QString &informativeText, QMessageBox::StandardButtons buttons,
                                                          bool switchLine = true);

    // 显示提问对话框
    static QMessageBox::StandardButton showMessageBoxQuestion(const QString &title, const QString &informativeText,
                                                              QMessageBox::StandardButtons buttons, bool switchLine = true);

    // 显示错误对话框
    static QMessageBox::StandardButton showMessageBoxError(const QString &title, const QString &informativeText, QMessageBox::StandardButtons buttons,
                                                           bool switchLine = true);

    // 显示警告对话框
    static QMessageBox::StandardButton showMessageBoxWarning(const QString &title, const QString &informativeText,
                                                             QMessageBox::StandardButtons buttons, bool switchLine = true);
};

} // namespace Utils

// ------------------ 按钮样式 ----------------------
class ButtonStyle : QProxyStyle
{
    Q_OBJECT
private:
    explicit ButtonStyle(QStyle *style = nullptr);
    ~ButtonStyle();

    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const override;

    int pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const override;

    // 声明为友元类
    friend class Utils::UiUtils;
};

// ----------------- 键盘控制 ------------------------
class KeyPressFilter : public QObject
{
private:
    QMessageBox *box = nullptr;
    QPushButton *okBtn = nullptr;
    QPushButton *cancelBtn = nullptr;

    KeyPressFilter(QMessageBox *b, QPushButton *ok, QPushButton *cancel)
        : box(b)
        , okBtn(ok)
        , cancelBtn(cancel)
    {}
    ~KeyPressFilter() override {}

    // 声明为友元类
    friend class Utils::UiUtils;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};
