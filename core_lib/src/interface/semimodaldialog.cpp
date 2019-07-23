#include "semimodaldialog.h"

SemiModalDialog::SemiModalDialog(Editor* editor, QWidget *parent) :
    QDialog(parent),
    mEditor(editor)
{
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_DeleteOnClose);
}

SemiModalDialog::~SemiModalDialog()
{

}
