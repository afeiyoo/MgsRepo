#include "configini.h"

#include "utils/configutils.h"
#include "utils/fileutils.h"

using namespace Utils;

ConfigIni::ConfigIni(QObject *parent)
    : Config{parent}
{}

ConfigIni::~ConfigIni() {}

void ConfigIni::load(const FileName &confPath)
{
    QString str = FileUtils::canonicalPath(confPath).toString();
    m_confUtil->init(str, ConfigUtils::INI);

    readConfig();
}
