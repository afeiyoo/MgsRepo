#include "qtcassert.h"

#include "Logger.h"

#include <QByteArray>

namespace Utils {

void writeAssertLocation(const char *msg)
{
    static bool goBoom = qEnvironmentVariableIsSet("QTC_FATAL_ASSERTS");
    if (goBoom)
        LOG_FATAL("SOFT ASSERT made fatal: %s", msg);
    else
        LOG_INFO("SOFT ASSERT: %s", msg);
}

} // namespace Utils
