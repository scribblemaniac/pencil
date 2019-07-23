#include "modalmanager.h"

#include "editor.h"
#include "nulldialog.h"
#include "toolmanager.h"

ModalManager::ModalManager(Editor* editor) : BaseManager(editor)
{

}

bool ModalManager::init()
{
    mCurrentDialog = new NullDialog();

    return true;
}

Status ModalManager::load(Object*)
{
    return Status::OK;
}

Status ModalManager::save(Object*)
{
    return Status::OK;
}

void ModalManager::setCurrentDialog(SemiModalDialog* dialog)
{
    mCurrentDialog = dialog;
}

SemiModalDialog* ModalManager::getCurrentDialog() const
{
    return mCurrentDialog;
}
