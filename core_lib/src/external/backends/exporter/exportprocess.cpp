#include "exportprocess.h"

Status ExportProcess::getStatus() const
{
    DebugDetails dd(mStatus.details());

    dd << QString("ExportProcess mTempDir: ").append(mTempDir.path())
       << QString("ExportProcess mCanceled: ").append(mCanceled);

    if (mStartTime >= 0)
    {
        double duration = static_cast<double>(clock() - mStartTime) / CLOCKS_PER_SEC;
        dd << QString("Export elapsed time: %1 seconds").arg(QString::number(duration, 'f', 2));
    }

    Status newStatus(mStatus);
    newStatus.setDetails(dd);
    return newStatus;
}

void ExportProcess::start()
{
    mStartTime = clock();
}

ThreadedExportProcess::ThreadedExportProcess(ExportProcessWorker *worker) :
    ExportProcess()
{
    mProcessThread = new QThread;
    mProcessWorker = worker;

    connect(worker, &ExportProcessWorker::majorProgressUpdate, this, &ThreadedExportProcess::majorProgressUpdate);
    connect(worker, &ExportProcessWorker::minorProgressUpdate, this, &ThreadedExportProcess::minorProgressUpdate);
    connect(worker, &ExportProcessWorker::progressMessageUpdate, this, &ThreadedExportProcess::progressMessageUpdate);
    connect(worker, &ExportProcessWorker::finished, this, &ThreadedExportProcess::finished);
}

ThreadedExportProcess::~ThreadedExportProcess()
{
    mProcessThread->quit();
    delete mProcessWorker;
    delete mProcessThread;
}

void ThreadedExportProcess::start()
{
    ExportProcess::start();

    // Run export process on its own thread
    // Operations on this thread are initated through the slots of the worker
    mProcessWorker->moveToThread(mProcessThread);
    mProcessThread->start();
}

Status ThreadedExportProcess::getStatus() const
{
    DebugDetails threadDebug;

    if (mProcessThread == nullptr)
    {
        threadDebug << "Export thread not initialized";
    }
    else
    {
        threadDebug << "Export thread status"
                    << QString("Thread is ").append(mProcessThread->isRunning() ? "running" : "not running")
                    << QString("Thread is ").append(mProcessThread->isFinished() ? "finished" : "not finished")
                    << QString("Priority: ").append(mProcessThread->priority())
                    << QString("Loop level: ").append(mProcessThread->loopLevel())
                    << QString("Stack size: ").append(mProcessThread->stackSize());
    }

    Status newStatus(mProcessWorker->getStatus());
    DebugDetails newDetails(newStatus.details());
    newDetails.collect(threadDebug);
    newStatus.setDetails(newDetails);

    return newStatus;
}
