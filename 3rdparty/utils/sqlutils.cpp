#include "sqlutils.h"

#include <QDebug>

#include <QFile>
#include <QXmlAttributes>
#include <QXmlDefaultHandler>
#include <QXmlInputSource>
#include <QXmlParseException>

// static 全局变量作用域为当前文件
static const char SQL_ID[] = "id";
static const char SQL_INCLUDED_DEFINE_ID[] = "defineId";
static const char SQL_TAGNAME_SQL[] = "sql";
static const char SQL_TAGNAME_SQLS[] = "sqls";
static const char SQL_TAGNAME_DEFINE[] = "define";
static const char SQL_TAGNAME_INCLUDE[] = "include";
static const char SQL_NAMESPACE[] = "namespace";

namespace Utils {

class SqlUtilsPrivate : public QXmlDefaultHandler
{
    M_D_CREATE(SqlUtils);

public:
    explicit SqlUtilsPrivate();
    ~SqlUtilsPrivate();
    QString buildKey(const QString &sqlNamespace, const QString &id);

protected:
    bool startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &attributes);
    bool endElement(const QString &namespaceURI, const QString &localName, const QString &qName);
    bool characters(const QString &str);
    bool fatalError(const QXmlParseException &exception);

private:
    bool loadSqlFiles(const FileNameList &sqlFiles);

private:
    QString m_sqlNamespace;
    QString m_currentText;
    QString m_currentSqlId;
    QString m_currentDefineId;
    QString m_currentIncludedDefineId;
    QHash<QString, QString> m_defines;
    QHash<QString, QString> m_sqls; // Key 是 id, value 是 SQL 语句
};

SqlUtilsPrivate::SqlUtilsPrivate()
    : QXmlDefaultHandler()
{}

SqlUtilsPrivate::~SqlUtilsPrivate() {}

QString SqlUtilsPrivate::buildKey(const QString &sqlNamespace, const QString &id)
{
    return sqlNamespace + "::" + id;
}

bool SqlUtilsPrivate::startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &attributes)
{
    Q_UNUSED(namespaceURI)
    Q_UNUSED(localName)

    // 1. 取得 SQL 得 xml 文档中的 namespace, sql id, include 的 defineId, include 的 id
    // 2. 如果是 <sql> 标签，清空 currentText
    // 3. 如果是 <define> 标签，清空 currentText
    if (SQL_TAGNAME_SQL == qName) {
        m_currentSqlId = attributes.value(SQL_ID);
        m_currentText = "";
    } else if (SQL_TAGNAME_INCLUDE == qName) {
        m_currentIncludedDefineId = attributes.value(SQL_INCLUDED_DEFINE_ID);
    } else if (SQL_TAGNAME_DEFINE == qName) {
        m_currentDefineId = attributes.value(SQL_ID);
        m_currentText = "";
    } else if (SQL_TAGNAME_SQLS == qName) {
        m_sqlNamespace = attributes.value(SQL_NAMESPACE);
    }

    return true;
}

bool SqlUtilsPrivate::endElement(const QString &namespaceURI, const QString &localName, const QString &qName)
{
    Q_UNUSED(namespaceURI)
    Q_UNUSED(localName)

    // 1. 如果是 <sql> 标签，则插入 sqls
    // 2. 如果是 <include> 标签，则从 defines 里取其内容加入 sql
    // 3. 如果是 <define> 标签，则存入 defines
    if (SQL_TAGNAME_SQL == qName) {
        // 取到一个完整的 SQL 语句
        m_sqls.insert(buildKey(m_sqlNamespace, m_currentSqlId), m_currentText.simplified());
        m_currentText = "";
    } else if (SQL_TAGNAME_INCLUDE == qName) {
        QString defKey = buildKey(m_sqlNamespace, m_currentIncludedDefineId);
        QString def = m_defines.value(defKey);

        if (!def.isEmpty()) {
            m_currentText += def;
        } else {
            qInfo().noquote() << QString("未找到标签 ==> 标签名: %1").arg(defKey);
        }
    } else if (SQL_TAGNAME_DEFINE == qName) {
        m_defines.insert(buildKey(m_sqlNamespace, m_currentDefineId), m_currentText.simplified());
    }

    return true;
}

bool SqlUtilsPrivate::characters(const QString &str)
{
    m_currentText += str;
    return true;
}

bool SqlUtilsPrivate::fatalError(const QXmlParseException &exception)
{
    qInfo().noquote() << QString("解析错误 ==> 行: %1, 列: %2, 错误提示: %3")
                             .arg(exception.lineNumber())
                             .arg(exception.columnNumber())
                             .arg(exception.message());
    return false;
}

bool SqlUtilsPrivate::loadSqlFiles(const FileNameList &sqlFiles)
{
    for (const auto &fileName : sqlFiles) {
        QFile file(fileName.toString());
        QXmlInputSource inputSource(&file);
        QXmlSimpleReader reader;
        reader.setContentHandler(this);
        reader.setErrorHandler(this);
        if (!reader.parse(inputSource))
            return false;

        m_defines.clear();
    }
    return true;
}

SqlUtils::SqlUtils(QObject *parent)
    : QObject{parent}
    , d_ptr(new SqlUtilsPrivate())
{
    Q_D(SqlUtils);
    d->q_ptr = this;
}

SqlUtils::~SqlUtils() = default;

QString SqlUtils::getSql(const QString &sqlNamespace, const QString &sqlId)
{
    Q_D(SqlUtils);
    QString sql = d->m_sqls.value(d->buildKey(sqlNamespace, sqlId));

    if (sql.isEmpty()) {
        qWarning().noquote() << QString("未找到对应 SQL ==> SQLID: %1::%2").arg(sqlNamespace).arg(sqlId);
    }

    return sql;
}

bool SqlUtils::loadSqlFiles(const FileNameList &sqlFiles)
{
    Q_D(SqlUtils);
    return d->loadSqlFiles(sqlFiles);
}
} // namespace Utils
