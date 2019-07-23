#ifndef SEMIMODALDIALOG_H
#define SEMIMODALDIALOG_H

#include <QDialog>
#include "editor.h"
#include "pencildef.h"

class SemiModalDialog : public QDialog
{
public:
    explicit SemiModalDialog(Editor* editor, QWidget *parent = nullptr);
    virtual ~SemiModalDialog();

    virtual bool blockToolSwitch(ToolType toolType) { return true; }

    virtual bool isNull() { return false; }

protected:
    Editor* mEditor = nullptr;
};

#endif // SEMIMODALDIALOG_H
