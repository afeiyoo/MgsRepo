#ifndef UIHELPER_H
#define UIHELPER_H

#include <QKeyEvent>
#include <QMessageBox>
#include <QObject>
#include <QProxyStyle>
#include <QPushButton>

class UiHelper : public QObject
{
    Q_OBJECT
public:
    explicit UiHelper(QObject *parent = nullptr);
    ~UiHelper() override {}

    // 显示提示对话框
    static QMessageBox::StandardButton showMessageBoxInfo(const QString &title,
                                                          const QString &informativeText,
                                                          QMessageBox::StandardButtons buttons);

    // 显示提问对话框
    static QMessageBox::StandardButton showMessageBoxQuestion(const QString &title,
                                                              const QString &informativeText,
                                                              QMessageBox::StandardButtons buttons);
};

// ------------------ 按钮样式 ----------------------
class ButtonStyle : QProxyStyle
{
    Q_OBJECT
private:
    explicit ButtonStyle(QStyle *style = nullptr);
    ~ButtonStyle();

    void drawControl(ControlElement element,
                     const QStyleOption *option,
                     QPainter *painter,
                     const QWidget *widget) const override;

    int pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const override;

    // 声明为友元类
    friend class UiHelper;
};

// ----------------- 键盘控制 ------------------------
class KeyPressFilter : public QObject
{
private:
    QMessageBox *box;
    QPushButton *okBtn;
    QPushButton *cancelBtn;

    KeyPressFilter(QMessageBox *b, QPushButton *ok, QPushButton *cancel)
        : box(b)
        , okBtn(ok)
        , cancelBtn(cancel)
    {}

    // 声明为友元类
    friend class UiHelper;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // UIHELPER_H
