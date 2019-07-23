#ifndef PEGBARALIGNMENTDIALOG_H
#define PEGBARALIGNMENTDIALOG_H

#include "editor.h"
#include "semimodaldialog.h"

namespace Ui {
class PegBarAlignmentDialog;
}

class PegBarAlignmentDialog : public SemiModalDialog
{
    Q_OBJECT

public:
    explicit PegBarAlignmentDialog(Editor* editor, QWidget *parent = nullptr);
    ~PegBarAlignmentDialog();

    void setLayerList(QStringList layerList);
    QStringList getLayerList();
    int getRefKey() {return refkey; }
    QString getRefLayer() {return refLayer; }
    void setLabRefKey();

    void setAreaSelected(bool b);
    void setReferenceSelected(bool b);
    void setLayerSelected(bool b);

    void updatePegRegLayers();
    void updatePegRegDialog();
    void alignPegs();

    //virtual bool blockToolSwitch(ToolType toolType) override;

public slots:
    void setBtnAlignEnabled();
    void setRefLayer(QString s);
    void setRefKey(int i);

signals:
    void closedialog();

private:
    Ui::PegBarAlignmentDialog *ui;
    QStringList mLayernames;
    bool areaSelected = false;
    bool referenceSelected = false;
    bool layerSelected = false;

    void closeClicked();
    void alignClicked();
    void layerListUpdate();
    QString refLayer = "";
    int refkey = 0;
};

#endif // PEGBARALIGNMENTDIALOG_H
