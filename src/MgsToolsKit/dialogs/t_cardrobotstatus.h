#pragma once

#include "ElaDialog.h"

#include <QGroupBox>

class ElaComboBox;
class ElaLineEdit;
class ElaPushButton;
class T_CardRobotStatus : public ElaDialog
{
    Q_OBJECT
public:
    explicit T_CardRobotStatus(QWidget *parent = nullptr);
    ~T_CardRobotStatus() override;

    void initContent();

    void initRobotStatus();

private:
    QGroupBox *makeGroupBox(const QString &title);

    // 卡机状态
    ElaComboBox *m_robotStatusComboBox1 = nullptr;
    ElaComboBox *m_robotStatusComboBox2 = nullptr;
    ElaComboBox *m_robotStatusComboBox3 = nullptr;
    ElaComboBox *m_robotStatusComboBox4 = nullptr;

    // 卡夹状态
    ElaComboBox *m_bagStatusComboBox1 = nullptr;
    ElaComboBox *m_bagStatusComboBox2 = nullptr;
    ElaComboBox *m_bagStatusComboBox3 = nullptr;
    ElaComboBox *m_bagStatusComboBox4 = nullptr;

    // 卡数量
    ElaLineEdit *m_cardNumEdit1 = nullptr;
    ElaLineEdit *m_cardNumEdit2 = nullptr;
    ElaLineEdit *m_cardNumEdit3 = nullptr;
    ElaLineEdit *m_cardNumEdit4 = nullptr;

    // 轨道有卡状态
    ElaComboBox *m_trackStatusComboBox1 = nullptr;
    ElaComboBox *m_trackStatusComboBox2 = nullptr;
    ElaComboBox *m_trackStatusComboBox3 = nullptr;
    ElaComboBox *m_trackStatusComboBox4 = nullptr;

    ElaPushButton *m_confirmButton = nullptr;
};
