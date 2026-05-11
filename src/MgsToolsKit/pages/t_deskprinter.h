#pragma once

#include "t_basepage.h"

class ElaLineEdit;
class ElaPushButton;
class ElaPlainTextEdit;
class ElaRadioButton;
class T_DeskPrinter : public T_BasePage
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit T_DeskPrinter(QWidget *parent = nullptr);
    ~T_DeskPrinter() override;

    void initContent();

public slots:
    void onPostToPrinter(bool checked = false);

private:
    ElaLineEdit *m_urlEdit = nullptr;
    ElaPushButton *m_sendButton = nullptr;
    ElaPushButton *m_inputButton = nullptr;
    ElaPlainTextEdit *m_inputEdit = nullptr;
    ElaPlainTextEdit *m_logEdit = nullptr;
    ElaPushButton *m_logClearButton = nullptr;
    ElaRadioButton *m_gbkButton = nullptr;
    ElaRadioButton *m_utfButton = nullptr;

    qint64 m_lastSendTime = 0;
};
