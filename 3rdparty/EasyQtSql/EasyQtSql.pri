QT *= sql

HEADERS += \
    $$PWD/EasyQtSql.h \
    $$PWD/EasyQtSql_Transaction.h \
    $$PWD/EasyQtSql_NonQueryResult.h \
    $$PWD/EasyQtSql_QueryResult.h \
    $$PWD/EasyQtSql_DBException.h \
    $$PWD/EasyQtSql_InsertQuery.h \
    $$PWD/EasyQtSql_DeleteQuery.h \
    $$PWD/EasyQtSql_PreparedQuery.h \
    $$PWD/EasyQtSql_ParamDirectionWrapper.h \
    $$PWD/EasyQtSql_UpdateQuery.h \
    $$PWD/EasyQtSql_SqlFactory.h \
    $$PWD/EasyQtSql_Util.h

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
