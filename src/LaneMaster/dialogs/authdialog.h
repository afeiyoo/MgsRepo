#pragma once

#include <QDialog>
#include <QLabel>

class ElaLineEdit;
class ElaPushButton;
class AuthDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AuthDialog(QWidget *parent = nullptr);
    ~AuthDialog() override;

    QString getID();
    void setID(const QString &id);
    QString getName();
    void setName(const QString &name);
    QString getPassWord();
    void setPassWord(const QString &passwd);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    void initUi();

private:
    QWidget *m_mainWidget = nullptr;
    // 工号输入
    ElaLineEdit *m_idEdit = nullptr;
    // 姓名输入
    ElaLineEdit *m_nameEdit = nullptr;
    // 密码输入
    ElaLineEdit *m_passwordEdit = nullptr;
    // 按钮组
    ElaPushButton *m_yesButton = nullptr;
    ElaPushButton *m_cancelButton = nullptr;
};
