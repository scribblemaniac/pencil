#ifndef EXPORTBACKEND_H
#define EXPORTBACKEND_H

#include "backend.h"

#include <QObject>

class QFile;

class ExportBackend : public virtual Backend
{
    Q_OBJECT
public:
    explicit ExportBackend(QObject *parent = nullptr);
};

#endif // EXPORTBACKEND_H
