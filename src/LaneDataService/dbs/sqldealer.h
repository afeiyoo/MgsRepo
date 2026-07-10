#pragma once

#include <QHash>
#include <QString>

class QFile;
class QXmlStreamReader;

/*
SQL 文件的定义
1. <sqls> 必须有 namespace
2. [<define>]*: <define> 必须在 <sql> 前定义，必须有 id 属性才有意义，否则不能被引用
3. [<sql>]*: <sql> 必须有 id 属性才有意义，<sql> 里可以用 <include defineId="define_id"> 引用 <define> 的内容

Example:
<sqls namespace="User">
    <define id="fields">id, username, password, email, mobile</define>

    <sql id="findByUserId">
        SELECT <include defineId="fields"/> FROM user WHERE id=%1
    </sql>
</sqls>
*/

class SqlDealer
{
public:
    SqlDealer();
    ~SqlDealer();

    QString getSql(const QString &sqlNamespace, const QString &sqlId);
    bool loadSqlFiles();

private:
    QString buildKey(const QString &sqlNamespace, const QString &id) const;
    bool parseSqlElement(QXmlStreamReader &reader);

private:
    // Key 是 id, value 是 SQL 语句
    QHash<QString, QString> sqls;
    QHash<QString, QString> defines;
    QString sqlNamespace;
};
