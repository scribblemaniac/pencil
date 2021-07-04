#include "exportmanager.h"

#include "ffmpegbackend.h"

const std::array<ExportBackend*,1> ExportManager::cBackends = { new FFmpegBackend };

ExportManager::ExportManager(Editor* editor) : BaseManager(editor)
{

}

ExportManager::~ExportManager()
{

}

bool ExportManager::init()
{
    return true;
}

Status ExportManager::load(Object*)
{
    return Status::OK;
}

Status ExportManager::save(Object*)
{
    return Status::OK;
}

ExportProcess* ExportManager::createExportProcess(const Object* obj, ExportMovieDesc &desc)
{
    for (ExportBackend* backend : cBackends)
    {
        if (backend->isAvailable() && backend->canExport(desc))
        {
            return backend->exportObject(obj, desc);
        }
    }

    return nullptr;
}
