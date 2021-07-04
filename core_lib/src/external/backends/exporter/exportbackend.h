#ifndef EXPORTBACKEND_H
#define EXPORTBACKEND_H

#include "backend.h"

#include <QSet>
#include <QObject>

class Object;
class ExportProcess;
class ExportMovieDesc;

class ExportBackend : public virtual Backend
{
    Q_OBJECT
public:
    explicit ExportBackend(QObject *parent = nullptr);

    virtual bool isAvailable() const override = 0;
    virtual DebugDetails getDebugInfo() const override = 0;

    /** Begin exporting the object with this backend in a new thread.
      *
      * @param[in] obj An Object containing the animation to export.
      * @param[in] desc A structure containing all the export parameters.
      *            See ExportMovieDesc.
      *
      * @return Returns the thread on which the export is running. Export is
      *         complete when ExportProcess::isFinished returns true or when
      *         ExportProcess::status is not Status::Pending.
      */
    virtual ExportProcess* exportObject(const Object* obj,
                                        const ExportMovieDesc& desc) = 0;

    /** Check if this backend can export an object with the passed settings.
      *
      * This does not check if the backend is currently available, so exporting
      * with these settings may still not be possible even if this returns true.
      *
      * By default this function only checks if the extension is in the list of
      * supported types provided by the subclass (through mSupportedExportTypes).
      *
      * @param[in] desc The settings to check for compatibility with this
      *                 exporter.
      *
      * @return Returns true if this exporter can export the desired type,
      *         false otherwise.
      */
    virtual bool canExport(ExportMovieDesc &desc);

protected:
    /** A list of types this exporter can export to.
      *
      * Entries in this list should be the full file extensions of supported
      * types without the leading period and in all lower case letters. This
      * variable can change during the lifetime of the object if support for a
      * type changes.
      *
      * Types should not be removed if they cannot be exported simply because
      * isAvailable is false. If a backend cannot be accesed, this should
      * contain the best guess of what types would be supported if the backend
      * was available.
      *
      * This variable may not need to be set if ExportBackend::canExport is
      * overwritten.
      */
    QSet<QString> mSupportedExportTypes;
};

#endif // EXPORTBACKEND_H
