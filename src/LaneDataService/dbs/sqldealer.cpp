#include "sqldealer.h"

#include "Logger.h"
#include "utils/fileutils.h"

#include <QFile>
#include <QXmlStreamReader>

using namespace Utils;

namespace {
const char SQL_ID[] = "id";
const char SQL_INCLUDED_DEFINE_ID[] = "defineId";
const char SQL_TAGNAME_SQL[] = "sql";
const char SQL_TAGNAME_SQLS[] = "sqls";
const char SQL_TAGNAME_DEFINE[] = "define";
const char SQL_TAGNAME_INCLUDE[] = "include";
const char SQL_NAMESPACE[] = "namespace";
} // namespace

SqlDealer::SqlDealer() {}

SqlDealer::~SqlDealer() {}

QString SqlDealer::getSql(const QString &sqlNamespace, const QString &sqlId)
{
    QString sql = sqls.value(buildKey(sqlNamespace, sqlId));

    if (sql.isEmpty())
        LOG_ERROR().noquote() << QString("找不到 SQL 语句 - %1::%2").arg(sqlNamespace).arg(sqlId);

    return sql;
}

QString SqlDealer::buildKey(const QString &sqlNamespace, const QString &id) const
{
    return sqlNamespace + "::" + id;
}

bool SqlDealer::loadSqlFiles(const QString &sqlFilesDir)
{
    FileNameList sqlFiles = FileUtils::getFilesWithSuffix(FileName::fromString(sqlFilesDir), ".xml");

    foreach (auto sqlFile, sqlFiles) {
        LOG_INFO().noquote() << QString("加载 SQL 文件: %1").arg(sqlFile.fileName());

        QFile file(sqlFile.toString());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            LOG_ERROR().noquote() << QString("打开 SQL 文件失败: %1, %2").arg(file.fileName()).arg(file.errorString());
            return false;
        }

        sqlNamespace.clear();
        QXmlStreamReader reader(&file);

        while (!reader.atEnd()) {
            reader.readNext();

            if (!reader.isStartElement())
                continue;

            if (reader.name() == QLatin1String(SQL_TAGNAME_SQLS)) {
                sqlNamespace = reader.attributes().value(QLatin1String(SQL_NAMESPACE)).toString();
            } else if (reader.name() == QLatin1String(SQL_TAGNAME_DEFINE)) {
                QString defineId = reader.attributes().value(QLatin1String(SQL_ID)).toString();
                QString text = reader.readElementText(QXmlStreamReader::IncludeChildElements).simplified();
                defines.insert(buildKey(sqlNamespace, defineId), text);
            } else if (reader.name() == QLatin1String(SQL_TAGNAME_SQL)) {
                if (!parseSqlElement(reader))
                    return false;
            }
        }

        if (reader.hasError()) {
            LOG_ERROR().noquote() << QString("Parse error in %1 at line %2, column %3, message: %4")
                                         .arg(file.fileName())
                                         .arg(reader.lineNumber())
                                         .arg(reader.columnNumber())
                                         .arg(reader.errorString());
            return false;
        }
        defines.clear();
    }

    return true;
}

bool SqlDealer::parseSqlElement(QXmlStreamReader &reader)
{
    QString sqlId = reader.attributes().value(QLatin1String(SQL_ID)).toString();
    QString text;

    while (!reader.atEnd()) {
        reader.readNext();

        if (reader.isCharacters() || reader.isCDATA()) {
            text += reader.text().toString();
        } else if (reader.isStartElement()) {
            if (reader.name() == QLatin1String(SQL_TAGNAME_INCLUDE)) {
                QString defineId = reader.attributes().value(QLatin1String(SQL_INCLUDED_DEFINE_ID)).toString();
                QString defKey = buildKey(sqlNamespace, defineId);
                QString def = defines.value(defKey);

                if (!def.isEmpty()) {
                    text += def;
                } else {
                    LOG_WARNING().noquote() << QString("Cannot find define: %1").arg(defKey);
                }

                reader.skipCurrentElement();
            } else {
                text += reader.readElementText(QXmlStreamReader::IncludeChildElements);
            }
        } else if (reader.isEndElement() && reader.name() == QLatin1String(SQL_TAGNAME_SQL)) {
            sqls.insert(buildKey(sqlNamespace, sqlId), text.simplified());
            return true;
        }
    }

    return false;
}
