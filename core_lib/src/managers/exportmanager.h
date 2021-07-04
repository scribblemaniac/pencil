#ifndef EXPORTMANAGER_H
#define EXPORTMANAGER_H

#include "basemanager.h"

class ExportBackend;
class ExportProcess;
class ExportMovieDesc;

class ExportManager : public BaseManager
{
    Q_OBJECT
public:
    explicit ExportManager(Editor* editor);
    ~ExportManager() override;

    bool init() override;
    Status load(Object*) override;
    Status save(Object*) override;

    /** Creates an ExportProcess instance to handle exporting an object.
      *
      * This function will find the best exporter backend which is available
      * and supports the target export's file type and other settings. If
      * a backend is found, it will return a ExporterProcess object from that
      * backend. It is the caller's responsibility to start this process and
      * clean up after it is complete.
      *
      * @param[in] obj The object to be exported.
      * @param[in] desc The settings to export the object with.
      *
      * @return A new export process instance initialized with the provided
      *         object and export settings, or nullptr if there is no backend
      *         which can export with the specified settings.
      */
    ExportProcess* createExportProcess(const Object* obj, ExportMovieDesc &desc);

private:
    /** An array of export backends that can be potentially be used.
      *
      * If multiple backends support the same output type, the first supporting
      * backend in this array will take precedence.
      */
    const static std::array<ExportBackend*,1> cBackends;
};

#endif // EXPORTMANAGER_H
