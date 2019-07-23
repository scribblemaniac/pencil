#ifndef NULLDIALOG_H
#define NULLDIALOG_H

#include "semimodaldialog.h"

class NullDialog : public SemiModalDialog
{
public:
    NullDialog() : SemiModalDialog (nullptr, nullptr) {}

    virtual bool blockToolSwitch(ToolType toolType) { return false; }

    bool isNull() override { return true; }
};

#endif // NULLDIALOG_H
