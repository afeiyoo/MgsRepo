#pragma once

// Local
#include <AbstractStringAppender.h>

class AndroidAppender : public AbstractStringAppender
{
public:
    AndroidAppender();

    static int androidLogPriority(Logger::LogLevel);

protected:
    void append(const QDateTime &timeStamp,
                Logger::LogLevel logLevel,
                const char *file,
                int line,
                const char *function,
                const QString &category,
                const QString &message);
};
