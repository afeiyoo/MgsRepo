#pragma once

#include <QObject>

#include "vehrecognizer_global.h"

class VEHRECOGNIZER_EXPORT IVehRecognizer : public QObject
{
    Q_OBJECT
public:
    explicit IVehRecognizer(QObject *parent = nullptr)
        : QObject(parent)
    {}
    virtual ~IVehRecognizer() = default;

signals:
};

extern "C" VEHRECOGNIZER_EXPORT IVehRecognizer *createVehRecognizer();
extern "C" VEHRECOGNIZER_EXPORT void destroyVehRecognizer(IVehRecognizer *rec);
