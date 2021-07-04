#ifndef IMPORTBACKEND_H
#define IMPORTBACKEND_H

#include "backend.h"

#include <QObject>

class ImportBackend : public virtual Backend
{
    Q_OBJECT
public:
    explicit ImportBackend(QObject *parent = nullptr);
};

#endif // IMPORTBACKEND_H
