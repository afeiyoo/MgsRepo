/**
 * 用于加载 SQL 语句，用法.
 * Sqls::instance().loadSqlFiles();
 * qDebug() << Sqls::instance().getSql("User", "findByUserId");
 *
 * SQL 文件的定义
 * 1. <sqls> 必须有 namespace
 * 2. [<define>]*: <define> 必须在 <sql> 前定义，必须有 id 属性才有意义，否则不能被引用
 * 3. [<sql>]*: <sql> 必须有 id 属性才有意义，<sql> 里可以用 <include defineId="define_id"> 引用 <define> 的内容
 *
 * SQL 文件定义 Demo:
 * <sqls namespace="User">
 *  <define id="fields">id, username, password, email, mobile</define>
 *
 *  <sql id="findByUserId">
 *      SELECT <include defineId="fields"/> FROM user WHERE id=%1
 *  </sql>
 *
 *  <sql id="findAll">
 *      SELECT id, username, password, email, mobile FROM user
 *  </sql>
 *
 *  <sql id="insert">
 *      INSERT INTO user (username, password, email, mobile)
 *      VALUES (:username, :password, :email, :mobile)
 *  </sql>
 *
 *  <sql id="update">
 *      UPDATE user SET username=:username, password=:password,
 *          email=:email, mobile=:mobile
 *      WHERE id=:id
 *  </sql>
 * </sqls>
 */

#pragma once

#include <QObject>

#include "fileutils.h"
#include "stdafx.h"

namespace Utils {

class SqlUtilsPrivate;
class SqlUtils : public QObject
{
    Q_OBJECT
    M_Q_CREATE(SqlUtils);

public:
    explicit SqlUtils(QObject *parent = nullptr);
    ~SqlUtils() override;

    // 取得 SQL 语句
    QString getSql(const QString &sqlNamespace, const QString &sqlId);
    // 加载 SQL 语句存储文件
    bool loadSqlFiles(const FileNameList &sqlFiles);
};

} // namespace Utils
