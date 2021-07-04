#ifndef FFMPEGBACKEND_H
#define FFMPEGBACKEND_H

#include "exportbackend.h"
#include "importbackend.h"
#include "exportprocess.h"
#include "exportmoviedesc.h"

#include <QImage>
#include <QProcess>

class FFmpegExportProcess;
class LayerCamera;

class FFmpegBackend : /*public ImportBackend,*/ public ExportBackend
{
    Q_OBJECT
public:
    FFmpegBackend();

    bool isAvailable() const override;
    DebugDetails getDebugInfo() const override;

    // ImportBackend

    // ExportBackend

    bool canExport(ExportMovieDesc &desc) override;

    ExportProcess* exportObject(const Object* obj,
                                const ExportMovieDesc& desc) override;

    static QString ffmpegLocation();
};

class FFmpegExportProcess : public ThreadedExportProcess
{
    Q_OBJECT
public:
    explicit FFmpegExportProcess(const FFmpegBackend *backend,
                                 const Object *obj,
                                 const ExportMovieDesc &desc);
    virtual ~FFmpegExportProcess() override {}

    Status getStatus() const override;

public slots:
    void start() override;
    void cancel() override;

private:
    const FFmpegBackend *mBackend;
};

class FFmpegExportProcessWorker : public ExportProcessWorker
{
    Q_OBJECT
public:
    explicit FFmpegExportProcessWorker(const Object *obj, const ExportMovieDesc &desc);
public slots:
    void start();
    void stop() override;

    void generateGif();
    void assembleAudio();
    void generateMovie();

private slots:
    void initializeProcess(QStringList args);

    // TODO document (esp. must be readyRead)
    void handleProcessOutput();
    void generateNextFrame();
    void updateProgress();

    void handleProcessError(QProcess::ProcessError error);
    void handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void generateGifComplete(int exitCode = 0, QProcess::ExitStatus exitStatus = QProcess::NormalExit);
    void assembleAudioComplete(int exitCode = 0, QProcess::ExitStatus exitStatus = QProcess::NormalExit);
    void generateMovieComplete(int exitCode = 0, QProcess::ExitStatus exitStatus = QProcess::NormalExit);
    void finish();

private:
    const Object* mObj;
    const ExportMovieDesc mDesc;
    const QTemporaryDir mTempDir;
    QString mTempOutputPath;
    QString mFFmpegLoc;
    int mTotalFrames;

    QImage mFrameTemplate;
    LayerCamera* mCamera;
    QTransform mCentralizeCamera;
    QRect mCameraRect;
    /** Total number of frames that should be generated but not rendered at any one moment.
      *
      * Frames are generated useing a "sliding window" to reduce memory usage
      * while having a relatively small impact on speed. This basically
      * means that there is a maximum number of frames that can be waiting
      * to be encoded by ffmpeg at any one time. The default value is designed
      * to take up a maximum of about 1GB of memory.
      *
      * This window is ignored if it appears that the rendering has stalled.
      * For example, this scenario can occur if ffmpeg does not have enough
      * frames to flush the output. @see mFrameGenerationFails.
      *
      * This is not used for GIF export since all frames must be generated
      * at once to create a global palette to render frames with.
      */
    int mFrameGenerationWindow;
    /** Number of times in a row that frame generation has been delayed because of the sliding window.
      *
      * If this counter exceeds a fixed number (currently 10), a frame will be
      * generated even though it will exceed the frame generation window size.
      */
    int mFrameGenerationFails = 0;

    enum Operation {
        NONE,
        PALETTE, // Currently unused
        GIF,
        AUDIO,
        MOVIE
    };

    QProcess* mProcess;
    Operation mActiveOperation = NONE;
    /** Count of frames that have been drawn by Pencil2D and written to the ffmpeg process. */
    int mFramesGenerated = 0;

    /** Count of frames encoded and written to the destination file by ffmpeg. */
    int mFramesRendered = 0;
    /** Duration of audio rendered, in milliseconds */
    int mAudioRendered = 0;

    DebugDetails mDebug;
};

#endif // FFMPEGBACKEND_H
