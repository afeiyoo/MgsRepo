/*
  Copyright (c) 2010 Boris Moiseev (cyberbobs at gmail dot com)

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License version 2.1
  as published by the Free Software Foundation and appearing in the file
  LICENSE.LGPL included in the packaging of this file.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.
*/
#pragma once

#include "CuteLogger_global.h"
#include <AbstractStringAppender.h>

class CUTELOGGER_EXPORT ConsoleAppender : public AbstractStringAppender
{
public:
    ConsoleAppender();
    virtual QString format() const;
    void ignoreEnvironmentPattern(bool ignore);
    void onlyPrintSetLevel(bool onlyPrint);
protected:
    virtual void append(const QDateTime &timeStamp,
                        Logger::LogLevel logLevel,
                        const char *file,
                        int line,
                        const char *function,
                        const QString &category,
                        const QString &message);

private:
    bool m_ignoreEnvPattern;
    bool m_onlyPrintSetLevel; //sml 2025-08-11 新增，只输出对应等级的日志，如：注册等级为trace,只输出trace等级
};
