#ifndef MODALMANAGER_H
#define MODALMANAGER_H

#include "basemanager.h"

class SemiModalDialog;

class ModalManager : public BaseManager
{
public:
    ModalManager(Editor* editor);

    bool init() override;
    Status load(Object* o) override;
    Status save(Object* o) override;

    void setCurrentDialog(SemiModalDialog* dialog);
    SemiModalDialog* getCurrentDialog() const;

private:
    SemiModalDialog* mCurrentDialog;
};

#endif // MODALMANAGER_H
