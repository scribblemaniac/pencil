#include "exportbackend.h"

#include "exportmoviedesc.h"

ExportBackend::ExportBackend(QObject *parent) : Backend(parent)
{

}

bool ExportBackend::canExport(ExportMovieDesc &desc)
{
    QString type = desc.strFileName.mid(desc.strFileName.lastIndexOf('.')+1).toLower();
    if(type.isEmpty()) return false;
    return mSupportedExportTypes.contains(type);
}
