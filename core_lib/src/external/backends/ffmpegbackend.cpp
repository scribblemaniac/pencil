#include "ffmpegbackend.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QImage>
#include <QPainter>
#include <QStandardPaths>
#include <QTime>
#include <QTimer>
#include <QtMath>

#include "layercamera.h"
#include "layersound.h"
#include "object.h"
#include "soundclip.h"

FFmpegBackend::FFmpegBackend() : ExportBackend()
{
    mSupportedExportTypes = { "mp4", "webm", "avi", "gif" };
}

bool FFmpegBackend::isAvailable() const
{
    return QFile::exists(ffmpegLocation());
}

// TODO use when ffmpeg error occurs or add to start of FFmpegBackend::mDebug
DebugDetails FFmpegBackend::getDebugInfo() const
{
    DebugDetails dd;
    bool available = isAvailable();
    QString ffmpegPath = ffmpegLocation();
    dd << QString("FFmpeg backend (%1)").arg(available ? "available" : "unavailable")
       << QString("FFmpeg location: ").append(ffmpegPath);
    if (available)
    {
        QProcess process;
        process.setReadChannel(QProcess::StandardOutput);
        // FFmpeg writes to stderr only for some reason, so we just read both channels together
        process.setProcessChannelMode(QProcess::MergedChannels);
        process.start(ffmpegPath, { "-version" });
        if (!process.waitForFinished())
        {
            process.terminate();
            if (process.state() != QProcess::Running)
            {
                process.kill();
            }
        }
        QString output = process.readAll();
        dd << QString("FFmpeg version information: ").append(output);
    }
    return dd;
}

bool FFmpegBackend::canExport(ExportMovieDesc &desc)
{
    bool ok = ExportBackend::canExport(desc);
    ok &= (!desc.strFileName.isEmpty());
    ok &= (desc.startFrame > 0);
    ok &= (desc.endFrame >= desc.startFrame);
    ok &= (desc.fps > 0);
    ok &= (!desc.strCameraName.isEmpty());

    return ok;
}

// ImportBackend

// ExportBackend

ExportProcess* FFmpegBackend::exportObject(const Object *obj, const ExportMovieDesc &desc)
{
    return new FFmpegExportProcess(this, obj, desc);
}

QString FFmpegBackend::ffmpegLocation()
{
    #ifdef _WIN32
        return QApplication::applicationDirPath() + "/plugins/ffmpeg.exe";
    #elif __APPLE__
        return QApplication::applicationDirPath() + "/plugins/ffmpeg";
    #else
        QString ffmpegPath = QStandardPaths::findExecutable(
            "ffmpeg",
            QStringList()
            << QApplication::applicationDirPath() + "/plugins"
            << QApplication::applicationDirPath() + "/../plugins" // linuxdeployqt in FHS-like mode
        );
        if (!ffmpegPath.isEmpty())
        {
            return ffmpegPath;
        }
        return QStandardPaths::findExecutable("ffmpeg"); // ffmpeg is a standalone project.
    #endif
}

FFmpegExportProcess::FFmpegExportProcess(const FFmpegBackend *backend, const Object *obj, const ExportMovieDesc &desc) :
    ThreadedExportProcess(new FFmpegExportProcessWorker(obj, desc)), mBackend(backend)
{

}

Status FFmpegExportProcess::getStatus() const
{
    Status newStatus(ThreadedExportProcess::getStatus());
    if (mBackend != nullptr)
    {
        DebugDetails newDetails(newStatus.details());
        newDetails.collect(mBackend->getDebugInfo());
        newStatus.setDetails(newDetails);
    }

    return newStatus;
}

void FFmpegExportProcess::start()
{
    ThreadedExportProcess::start();

    emit progressMessageUpdate(tr("Initializing..."));

    // Single shot so (that it will run on mProcessThread rather than this thread
    QTimer::singleShot(0, static_cast<FFmpegExportProcessWorker *>(mProcessWorker), &FFmpegExportProcessWorker::start);
}

void FFmpegExportProcess::cancel()
{
    ExportProcess::cancel();

    mProcessWorker->stop();
}

FFmpegExportProcessWorker::FFmpegExportProcessWorker(const Object *obj, const ExportMovieDesc &desc) :
    ExportProcessWorker(), mObj(obj), mDesc(desc)
{
    mTotalFrames = mDesc.endFrame - mDesc.startFrame + 1;
    if (!mTempDir.isValid())
    {
        mStatus = Status::FAIL;
        mStatus.setTitle(tr("Something went wrong"));
        mStatus.setDescription(tr("Could not create temporary directory."));
        mDebug << QString("QTemporaryDir error: ").append(mTempDir.errorString());
        finish();
    }

    mFFmpegLoc = FFmpegBackend::ffmpegLocation();
    qDebug() << mFFmpegLoc;
    if (!QFile::exists(mFFmpegLoc))
    {
#ifdef _WIN32
        qCritical() << "Please place ffmpeg.exe at the path " << mFFmpegLoc;
#else
        qCritical() << "Please place ffmpeg at the path " << mFFmpegLoc;
#endif
        mStatus = Status::ERROR_FFMPEG_NOT_FOUND;
        finish();
    }
}

void FFmpegExportProcessWorker::stop()
{
    // Stop ffmpeg if it is currently running
    // TODO Fix race condition. No process -> skip this -> process is created -> finish is called without terminating process
    if (mProcess && mProcess->state() != QProcess::NotRunning)
    {
        mProcess->terminate();
        if (mProcess->state() != QProcess::Running)
        {
            mProcess->kill();
        }
    }
    else {
        finish();
    }

    if (mStatus == Status::PENDING || mStatus.ok())
    {
        mStatus = Status::CANCELED;
    }
}

void FFmpegExportProcessWorker::finish()
{
    emit minorProgressUpdate(1.f);
    emit majorProgressUpdate(1.f);
    emit progressMessageUpdate(tr("Done"));

    if (mStatus == Status::PENDING)
    {
        mStatus = Status::OK;
    }

    if (mProcess && mProcess->isReadable())
    {
        // Read any remaining output from ffmpeg
        QString output(mProcess->readAll());
        QStringList sList = output.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
        for (const QString& s : sList)
        {
            qDebug() << "[ffmpeg]" << s;
            mDebug << s;
        }
    }
    mDebug << QString("Export complete");

    mStatus.setDetails(mDebug);
    emit finished();
}

void FFmpegExportProcessWorker::generateGif()
{
    emit progressMessageUpdate(tr("Generating GIF..."));

    // QDir::filePath is used because QTemporaryDir::file path is only available in 5.9+
    mTempOutputPath = QDir(mTempDir.path()).filePath("output.gif");

    // Build FFmpeg command

    QStringList args = {"-f", "rawvideo", "-pixel_format", "bgra"};
    args << "-video_size" << QString("%1x%2").arg(mDesc.exportSize.width()).arg(mDesc.exportSize.height());
    args << "-framerate" << QString::number(mDesc.fps);

    args << "-i" << "-";

    args << "-y";

    args << "-filter_complex" << "[0:v]palettegen [p]; [0:v][p] paletteuse";

    args << "-loop" << (mDesc.loop ? "0" : "-1");
    args << mTempOutputPath;

    // Run FFmpeg command

    mActiveOperation = GIF;
    initializeProcess(args);
    connect(mProcess, &QProcess::started, this, &FFmpegExportProcessWorker::generateNextFrame);
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(mProcess, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &FFmpegExportProcessWorker::generateGifComplete);
    mProcess->start();
}

void FFmpegExportProcessWorker::assembleAudio()
{
    emit progressMessageUpdate(tr("Assembling audio..."));

    // QDir::filePath is used because QTemporaryDir::file path is only available in 5.9+
    QString tempAudioPath = QDir(mTempDir.path()).filePath("tmpaudio.wav");
    qDebug() << "TempAudio=" << tempAudioPath;

    std::vector<SoundClip*> allSoundClips;
    std::vector<LayerSound*> allSoundLayers = mObj->getLayersByType<LayerSound>();
    for (LayerSound* layer : allSoundLayers)
    {
        layer->foreachKeyFrame([&allSoundClips](KeyFrame* key)
        {
            if (!key->fileName().isEmpty())
            {
                allSoundClips.push_back(static_cast<SoundClip*>(key));
            }
        });
    }

    if (allSoundClips.empty())
    {
        assembleAudioComplete();
        return;
    }

    int clipCount = 0;

    QString filterComplex, amergeInput, panChannelLayout;
    QStringList args;

    int wholeLen = qCeil((mDesc.endFrame - mDesc.startFrame) * 44100.0 / mDesc.fps);
    for (auto clip : allSoundClips)
    {
        // Add sound file as input
        args << "-i" << clip->fileName();

        // Offset the sound to its correct position
        // See https://superuser.com/questions/716320/ffmpeg-placing-audio-at-specific-location
        filterComplex += QString("[%1:a:0] aformat=sample_fmts=fltp:sample_rates=44100:channel_layouts=mono,volume=1,adelay=%2S|%2S,apad=whole_len=%3[ad%1];")
                    .arg(clipCount).arg(qRound(44100.0 * (clip->pos() - 1) / mDesc.fps)).arg(wholeLen);
        amergeInput += QString("[ad%1]").arg(clipCount);
        panChannelLayout += QString("c%1+").arg(clipCount);

        clipCount++;
    }
    // Remove final '+'
    panChannelLayout.chop(1);
    // Output arguments
    // Mix audio
    args << "-filter_complex" << QString("%1%2 amerge=inputs=%3, pan=mono|c0=%4 [out]")
            .arg(filterComplex).arg(amergeInput).arg(clipCount).arg(panChannelLayout);
    // Convert audio file: 44100Hz sampling rate, stereo, signed 16 bit little endian
    // Supported audio file types: wav, mp3, ogg... ( all file types supported by ffmpeg )
    args << "-ar" << "44100" << "-acodec" << "pcm_s16le" << "-ac" << "2" << "-map" << "[out]" << "-y";
    // Trim audio
    args << "-ss" << QString::number((mDesc.startFrame - 1) / static_cast<double>(mDesc.fps));
    args << "-to" << QString::number(mDesc.endFrame / static_cast<double>(mDesc.fps));
    // Output path
    args << tempAudioPath;

    mActiveOperation = AUDIO;
    initializeProcess(args);
    connect(mProcess, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &FFmpegExportProcessWorker::assembleAudioComplete);
    mProcess->start();
}

void FFmpegExportProcessWorker::generateMovie()
{
    emit progressMessageUpdate(tr("Generating movie..."));

    QString extension = mDesc.strFileName.mid(mDesc.strFileName.lastIndexOf('.'));
    // QDir::filePath is used because QTemporaryDir::file path is only available in 5.9+
    mTempOutputPath = QDir(mTempDir.path()).filePath(QString("output").append(extension));

    // Build FFmpeg command

    //int exportFps = mDesc.videoFps;
    const QString tempAudioPath = QDir(mTempDir.path()).filePath("tmpaudio.wav");

    QStringList args = {"-f", "rawvideo", "-pixel_format", "bgra"};
    args << "-video_size" << QString("%1x%2").arg(mCameraRect.width()).arg(mCameraRect.height());
    args << "-framerate" << QString::number(mDesc.fps);

    //args << "-r" << QString::number(exportFps);
    args << "-i" << "-";
    args << "-threads" << (QThread::idealThreadCount() == 1 ? "0" : QString::number(QThread::idealThreadCount()));

    if (QFile::exists(tempAudioPath))
    {
        args << "-i" << tempAudioPath;
    }

    if (extension.compare(".apng", Qt::CaseInsensitive) == 0)
    {
        args << "-plays" << (mDesc.loop ? "0" : "1");
    }

    if (extension.compare(".mp4", Qt::CaseInsensitive) == 0)
    {
        args << "-pix_fmt" << "yuv420p";
    }

    if (extension.compare(".avi", Qt::CaseInsensitive) == 0)
    {
        args << "-q:v" << "5";
    }

    args << "-y";
    args << mTempOutputPath;

    mActiveOperation = MOVIE;
    initializeProcess(args);
    connect(mProcess, &QProcess::started, this, &FFmpegExportProcessWorker::generateNextFrame);
    connect(mProcess, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &FFmpegExportProcessWorker::generateMovieComplete);
    mProcess->start();
}

void FFmpegExportProcessWorker::initializeProcess(QStringList args)
{
    mProcess = new QProcess();
    mProcess->setReadChannel(QProcess::StandardOutput);
    // FFmpeg writes to stderr only for some reason, so we just read both channels together
    mProcess->setProcessChannelMode(QProcess::MergedChannels);
    connect(mProcess, &QProcess::readyRead, this, &FFmpegExportProcessWorker::handleProcessOutput);
    connect(mProcess, &QProcess::errorOccurred, this, &FFmpegExportProcessWorker::handleProcessError);
    connect(mProcess, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &FFmpegExportProcessWorker::handleProcessFinished);
    mProcess->setProgram(mFFmpegLoc);
    mProcess->setArguments(args);
    mDebug << QStringLiteral("Initialized command: %1 %2").arg(mFFmpegLoc).arg(args.join(' '));
}

void FFmpegExportProcessWorker::updateProgress()
{
    if (mDesc.strFileName.endsWith(".gif"))
    {
        float progress = (mFramesGenerated + mFramesRendered) / (2. * mTotalFrames);
        emit majorProgressUpdate(progress);
        emit minorProgressUpdate(progress);
    }
    else
    {
        if (mActiveOperation == AUDIO)
        {
            // Audio assembly is the first 25% of movie export progress
            float minorProgress = mAudioRendered / (static_cast<float>(mTotalFrames) / mDesc.fps);
            float majorProgress = 0.25 * minorProgress;
            emit majorProgressUpdate(majorProgress);
            emit minorProgressUpdate(minorProgress);
        }
        else if (mActiveOperation != MOVIE && mFramesGenerated == 0) // Assume unstarted
        {
            emit majorProgressUpdate(0.f);
            emit minorProgressUpdate(0.f);
        }
        else
        {
            float minorProgress = (mFramesGenerated + mFramesRendered) / (2. * mTotalFrames);
            float majorProgress = 0.25 + (0.75 * minorProgress);
            emit majorProgressUpdate(majorProgress);
            emit minorProgressUpdate(minorProgress);
        }
    }
}

void FFmpegExportProcessWorker::start()
{
    updateProgress();

    qDebug() << "OutFile: " << mDesc.strFileName;

    /* We create an image with the correct dimensions and background
     * color here and then copy this and draw over top of it to
     * generate each frame. This is faster than having to generate
     * a new background image for each frame.
     */
    mFrameTemplate = QImage(mDesc.exportSize, QImage::Format_ARGB32_Premultiplied);
    QColor bgColor = Qt::white;
    if (mDesc.alpha)
    {
        bgColor.setAlpha(0);
    }
    mFrameTemplate.fill(bgColor);

    // Set camera layer and constant properties
    mCamera = static_cast<LayerCamera*>(mObj->findLayerByName(mDesc.strCameraName, Layer::CAMERA));
    if (!mCamera)
    {
        mCamera = mObj->getLayersByType<LayerCamera>().front();
    }
    Q_ASSERT(mCamera);
    QSize camSize = mCamera->getViewSize();
    mCentralizeCamera.translate(camSize.width() / 2, camSize.height() / 2);
    mCameraRect = QRect(0, 0, camSize.width(), camSize.height());
    mFrameGenerationWindow = qMax(1, qFloor(1e9 / mFrameTemplate.byteCount()));

    minorProgressUpdate(0.f);
    if (mDesc.strFileName.endsWith(".gif", Qt::CaseInsensitive))
    {
        generateGif();
    }
    else
    {
        assembleAudio();
    }
}

void FFmpegExportProcessWorker::handleProcessOutput()
{
    QString output(mProcess->readAll());
    QStringList sList = output.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
    for (const QString& s : sList)
    {
        qDebug() << "[ffmpeg]" << s;
        mDebug << s;
    }
    if (mActiveOperation == AUDIO)
    {
        if (output.contains("time="))
        {
            int timeIndex = output.indexOf("time=");
            QTime timeRendered = QTime::fromString(output.mid(timeIndex+5, 11).append("0"), "hh:mm:ss.zzz");
            int audioRendered = timeRendered.msecsSinceStartOfDay();
            if (audioRendered != mAudioRendered)
            {
                mAudioRendered = audioRendered;
                updateProgress();
            }
        }
    }
    else
    {
        if (output.startsWith("frame="))
        {
            int framesRendered = output.mid(6, output.indexOf(' ')).toInt();
            if (framesRendered != mFramesRendered)
            {
                mFramesRendered = framesRendered;
                updateProgress();
            }
        }
    }
}

void FFmpegExportProcessWorker::handleProcessError(QProcess::ProcessError error)
{
    mDebug << QString("Encountered ProcessError #%1: %2").arg(error).arg(mProcess->errorString());
    switch(error)
    {
    case QProcess::FailedToStart:
        mStatus = Status::FAIL;
        mStatus.setTitle(tr("Something went wrong"));
        mStatus.setDescription(tr("Couldn't start the video backend, please try again."));
        stop();
        break;
    case QProcess::Crashed:
        mStatus = Status::FAIL;
        mStatus.setTitle(tr("Something went wrong"));
        mStatus.setDescription(tr("Looks like our video backend did not exit normally. Your movie may not have exported correctly. Please try again and report this if it persists."));
        mDebug << QString("Exit status: ").append(QProcess::NormalExit ? "NormalExit": "CrashExit")
               << QString("Exit code: %1").arg(mProcess->exitCode());
        stop();
        break;
    case QProcess::Timedout:
        Q_ASSERT_X(false, "FFmpeg Backend", "Something timed out. Blocking waitFor functions should not be called in this backend.");
        break;
    case QProcess::WriteError:
        Q_ASSERT_X(false, "FFmpeg Backend", "Write error occured in QProcess, perhaps a write was attempted before or after the process runs?");
        break;
    case QProcess::ReadError:
        Q_ASSERT_X(false, "FFmpeg Backend", "Read error occured in QProcess, perhaps a read was attempted before or after the process runs?");
        break;
    case QProcess::UnknownError:
    default:
        mStatus = Status::FAIL;
        mStatus.setTitle(tr("Something went wrong"));
        mStatus.setDescription(tr("The video backend encountered an unknown error. Export likely failed."));
        stop();
    }
}

void FFmpegExportProcessWorker::handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode)
    Q_UNUSED(exitStatus)

    mActiveOperation = NONE;
}

void FFmpegExportProcessWorker::FFmpegExportProcessWorker::generateNextFrame()
{
    // Check if frame generation complete
    if (mFramesGenerated > mTotalFrames)
    {
        mProcess->closeWriteChannel();
        return;
    }

    // Check if sliding window is full
    if (mActiveOperation == MOVIE &&
        mFrameGenerationFails < 10 &&
        mFramesGenerated - mFramesRendered >= mFrameGenerationWindow)
    {
        mFrameGenerationFails++;
        QTimer::singleShot(100, this, &FFmpegExportProcessWorker::generateNextFrame);
        return;
    }

    int bytesWritten;

    QImage imageToExport = mFrameTemplate.copy();
    QPainter painter(&imageToExport);

    QTransform view = mCamera->getViewAtFrame(mDesc.startFrame + mFramesGenerated);
    painter.setWorldTransform(view * mCentralizeCamera);
    painter.setWindow(mCameraRect);

    mObj->paintImage(painter, mDesc.startFrame + mFramesGenerated, false, true);

    // Should use sizeInBytes instead of byteCount to support large images,
    // but this is only supported in QT 5.10+
    bytesWritten = mProcess->write(reinterpret_cast<const char*>(imageToExport.constBits()), imageToExport.byteCount());
    Q_ASSERT(bytesWritten == imageToExport.byteCount());

    mFramesGenerated++;
    updateProgress();
    mFrameGenerationFails = 0;
    QTimer::singleShot(0, this, &FFmpegExportProcessWorker::generateNextFrame);
}

void FFmpegExportProcessWorker::generateGifComplete(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode)
    // Non-normal exits are handled by handleProcessError
    if(exitStatus != QProcess::NormalExit) return;

    QFile::rename(mTempOutputPath, mDesc.strFileName);

    finish();
}

void FFmpegExportProcessWorker::assembleAudioComplete(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode)
    // Non-normal exits are handled by handleProcessError
    if(exitStatus != QProcess::NormalExit) return;

    generateMovie();
}

void FFmpegExportProcessWorker::generateMovieComplete(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode)
    // Non-normal exits are handled by handleProcessError
    if(exitStatus != QProcess::NormalExit) return;

    QFile::rename(mTempOutputPath, mDesc.strFileName);

    finish();
}
