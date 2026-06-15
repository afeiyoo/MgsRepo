#pragma once

#include <QObject>

class Environment : public QObject
{
    Q_OBJECT
public:
    explicit Environment(QObject *parent = nullptr);
    ~Environment() override;

public:
    bool m_isInShifted = false; // 是否已登班
};
