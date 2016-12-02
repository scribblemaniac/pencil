#ifndef TIMELINE2_H
#define TIMELINE2_H

#include <QDockWidget>
#include "basedockwidget.h"

namespace Ui {
class Timeline2;
}


class Timeline2 : public BaseDockWidget
{
    Q_OBJECT

public:
    explicit Timeline2(Editor* editor, QWidget* parent = 0);
    ~Timeline2();

    void initUI() override;
    void updateUI() override;

private:
    Ui::Timeline2* ui;
    Editor* mEditor;
};

#endif // TIMELINE2_H
