#include "pegbaralignmentdialog.h"
#include "ui_pegbaralignmentdialog.h"

#include <QListWidget>
#include <QListWidgetItem>

#include "layerbitmap.h"

PegBarAlignmentDialog::PegBarAlignmentDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PegBarAlignmentDialog)
{
    ui->setupUi(this);
}

PegBarAlignmentDialog::~PegBarAlignmentDialog()
{
    delete ui;
}

void PegBarAlignmentDialog::setLayerList(const QVector<LayerBitmap*> layers)
{
    QListWidgetItem *item;
    foreach (LayerBitmap *l, layers)
    {
        item = new QListWidgetItem(l->name());
        item->setData(Qt::UserRole, QVariant::fromValue<LayerBitmap*>(l));
        ui->lwLayers->addItem(item);
    }
}

QVector<LayerBitmap*> PegBarAlignmentDialog::getSelectedLayers() const
{
    QVector<LayerBitmap*> layers;
    foreach (const QListWidgetItem* item, ui->lwLayers->selectedItems())
    {
        layers.push_back(item->data(Qt::UserRole).value<LayerBitmap*>());
    }
    return layers;
}

void PegBarAlignmentDialog::setReferenceKeyText(const QString txt)
{
    ui->labRefKey->setText(txt);
}

