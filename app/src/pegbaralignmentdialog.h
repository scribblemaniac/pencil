#ifndef PEGBARALIGNMENTDIALOG_H
#define PEGBARALIGNMENTDIALOG_H

#include <QDialog>
#include <QStringList>

namespace Ui {
class PegBarAlignmentDialog;
}

class LayerBitmap;

class PegBarAlignmentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PegBarAlignmentDialog(QWidget *parent = nullptr);
    ~PegBarAlignmentDialog();

    void setLayerList(const QVector<LayerBitmap*> layers);
    QVector<LayerBitmap*> getSelectedLayers() const;
    void setReferenceKeyText(const QString txt);

private:
    Ui::PegBarAlignmentDialog *ui;
};

#endif // PEGBARALIGNMENTDIALOG_H
