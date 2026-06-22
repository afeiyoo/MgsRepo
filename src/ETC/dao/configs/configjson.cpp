#include "configjson.h"

#include "utils/configutils.h"
#include "utils/fileutils.h"

using namespace Utils;

ConfigJson::ConfigJson(QObject *parent)
    : Config{parent}
{}

ConfigJson::~ConfigJson() {}

void ConfigJson::load(const Utils::FileName &confPath)
{
    QString str = FileUtils::canonicalPath(confPath).toString();
    m_confUtil->init(str, ConfigUtils::JSON);

    readConfig();
}
