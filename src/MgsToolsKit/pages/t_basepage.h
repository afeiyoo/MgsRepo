#pragma once

#include "ElaScrollPage.h"

class T_BasePage : public ElaScrollPage
{
    Q_OBJECT
public:
    explicit T_BasePage(QWidget *parent = nullptr);
    ~T_BasePage() override;

protected:
    void createCustomWidget(QString desText);
};
