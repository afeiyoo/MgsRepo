#pragma once

#include <QtCore/qglobal.h>

#include <QObject>

// Q_D Q_Q指针变量快速创建
#define M_PRIVATE_CREATE_Q_H(TYPE, M) \
public: \
    void set##M(TYPE M); \
    TYPE get##M() const;

#define M_PRIVATE_CREATE_Q_CPP(CLASS, TYPE, M) \
    void CLASS::set##M(TYPE M) \
    { \
        Q_D(CLASS); \
        d->_p##M = M; \
    } \
    TYPE CLASS::get##M() const \
    { \
        return d_ptr->_p##M; \
    }

#define M_PRIVATE_CREATE_D(TYPE, M) \
private: \
    TYPE _p##M;

#define M_PRIVATE_CREATE(TYPE, M) \
public: \
    void set##M(TYPE M) \
    { \
        _p##M = M; \
    } \
    TYPE get##M() const \
    { \
        return _p##M; \
    } \
\
private: \
    TYPE _p##M;

// Q_Q Q_D使用
#define M_Q_CREATE(CLASS) \
protected: \
    explicit CLASS(CLASS##Private &dd, CLASS *parent = nullptr); \
    QScopedPointer<CLASS##Private> d_ptr; \
\
private: \
    Q_DISABLE_COPY(CLASS) \
    Q_DECLARE_PRIVATE(CLASS);

#define M_D_CREATE(CLASS) \
protected: \
    CLASS *q_ptr; \
\
private: \
    Q_DECLARE_PUBLIC(CLASS);

// 安全内存释放
#define SAFE_DELETE(p) \
    { \
        if (p) { \
            delete (p); \
            (p) = nullptr; \
        } \
    }
#define SAFE_DELETE_ARRAY(p) \
    { \
        if (p) { \
            delete[] (p); \
            (p) = nullptr; \
        } \
    }
