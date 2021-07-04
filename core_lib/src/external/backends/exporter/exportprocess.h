#ifndef EXPORTPROCESS_H
#define EXPORTPROCESS_H

#include <QTemporaryDir>
#include <QThread>

#include "pencilerror.h"

class ExportProcessWorker;

class ExportProcess : public QObject
{
    Q_OBJECT
public:
    ExportProcess() : QObject() {}

    virtual Status getStatus() const;

signals:
    // TODO document
    void majorProgressUpdate(float);
    void minorProgressUpdate(float);
    void progressMessageUpdate(QString);

    // TODO document
    void finished();

public slots:
    // TODO document
    virtual void start();
    virtual void cancel() { mCanceled = true; }

protected:
    Status mStatus = Status::PENDING;
    QTemporaryDir mTempDir;
    bool mCanceled = false;
    clock_t mStartTime = -1;
    DebugDetails mDebug;
};

class ThreadedExportProcess : public ExportProcess
{
    Q_OBJECT
public:
    Status getStatus() const override;

    explicit ThreadedExportProcess(ExportProcessWorker *worker);
    virtual ~ThreadedExportProcess();

public slots:
    virtual void start() override;

protected:
    QThread *mProcessThread;
    ExportProcessWorker *mProcessWorker;
};

class ExportProcessWorker : public QObject
{
    Q_OBJECT
public:
    Status getStatus() { return mStatus; }

signals:
    void majorProgressUpdate(float);
    void minorProgressUpdate(float);
    void progressMessageUpdate(QString);

    void finished();

public slots:
    virtual void stop() = 0;

protected:
    Status mStatus = Status::PENDING;
};

#endif // EXPORTPROCESS_H
