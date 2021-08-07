/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "importimageseqdialog.h"
#include "ui_importimageseqoptions.h"
#include "ui_importimageseqpreview.h"
#include "util.h"
#include "app_util.h"

#include "editor.h"
#include "predefinedsetmodel.h"
#include "layermanager.h"
#include "viewmanager.h"

#include <QProgressDialog>
#include <QMessageBox>
#include <QDir>
#include <QtDebug>
#include <QDialogButtonBox>
#include <QPushButton>

ImportImageSeqDialog::ImportImageSeqDialog(QWidget* parent, Mode mode, FileType fileType, ImportCriteria importCriteria) :
    ImportExportDialog(parent, mode, fileType), mParent(parent), mImportCriteria(importCriteria), mFileType(fileType)
{

    uiOptionsBox = new Ui::ImportImageSeqOptions;
    uiOptionsBox->setupUi(getOptionsGroupBox());

    uiGroupBoxPreview = new Ui::ImportImageSeqPreviewGroupBox;
    uiGroupBoxPreview->setupUi(getPreviewGroupBox());

    if (importCriteria == ImportCriteria::PredefinedSet) {
        setupPredefinedLayout();
    } else {
        setupLayout();
    }

    getDialogButtonBox()->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(false);
}

void ImportImageSeqDialog::setupLayout()
{

    hideInstructionsLabel(true);

    if (mFileType == FileType::GIF) {
        setWindowTitle(tr("Import Animated GIF"));
    } else {
        setWindowTitle(tr("Import image sequence"));
    }

    connect(uiOptionsBox->spaceSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &ImportImageSeqDialog::setSpace);
    connect(this, &ImportImageSeqDialog::filePathsChanged, this, &ImportImageSeqDialog::validateFiles);
}

void ImportImageSeqDialog::setupPredefinedLayout()
{
    setWindowTitle(tr("Import predefined keyframe set"));
    setInstructionsLabel(tr("Select an image that matches the criteria: MyFile000.png, eg. Joe001.png \n"
                         "The importer will search and find images matching the same criteria. You can see the result in the preview box below."));
    hideOptionsGroupBox(true);
    hidePreviewGroupBox(false);

    connect(this, &ImportImageSeqDialog::filePathsChanged, this, &ImportImageSeqDialog::updatePreviewList);
}

ImportImageSeqDialog::~ImportImageSeqDialog()
{
    if (uiOptionsBox) {
        delete uiOptionsBox;
    }
    if (uiGroupBoxPreview) {
        delete uiGroupBoxPreview;
    }
}

int ImportImageSeqDialog::getSpace()
{
    return uiOptionsBox->spaceSpinBox->value();
}

void ImportImageSeqDialog::updatePreviewList(const QStringList& list)
{
    Q_UNUSED(list)
    if (mImportCriteria == ImportCriteria::PredefinedSet)
    {
        const PredefinedKeySet& keySet = generatePredefinedKeySet();

        Status status = Status::OK;
        status = validateKeySet(keySet, list);

        QPushButton* okButton = getDialogButtonBox()->button(QDialogButtonBox::StandardButton::Ok);
        if (status == Status::FAIL)
        {
            QMessageBox::warning(mParent,
                                 status.title(),
                                 status.description(),
                                 QMessageBox::Ok,
                                 QMessageBox::Ok);
            okButton->setEnabled(false);
        } else {
            okButton->setEnabled(true);
        }
        setPreviewModel(keySet);
    }
}

const PredefinedKeySet ImportImageSeqDialog::generatePredefinedKeySet() const
{
    PredefinedKeySet keySet;
    const QList<QPair<QString, int> > fileSet = predefinedKeySetParams();

    for (int i = 0; i < fileSet.size(); i++)
    {
        keySet.insert(fileSet[i].second, fileSet[i].first);
    }

    if (!fileSet.empty())
    {
        QString prefix = QRegularExpression("^(.*?)(\\d+)([^\\d\\.]*\\.[^\\.]+)$").match(QFileInfo(fileSet[0].first).baseName()).captured(1);
        keySet.setLayerName(prefix);
    }

    return keySet;
}

void ImportImageSeqDialog::setPreviewModel(const PredefinedKeySet& keySet)
{
    PredefinedSetModel* previewModel = new PredefinedSetModel(nullptr, keySet);
    uiGroupBoxPreview->tableView->setModel(previewModel);
    uiGroupBoxPreview->tableView->setColumnWidth(0, 500);
    uiGroupBoxPreview->tableView->setColumnWidth(1, 100);
}

ImportExportDialog::Mode ImportImageSeqDialog::getMode()
{
    return ImportExportDialog::Import;
}

FileType ImportImageSeqDialog::getFileType()
{
    return mFileType;
}

void ImportImageSeqDialog::setSpace(int number)
{
    QSignalBlocker b1(uiOptionsBox->spaceSpinBox);
    uiOptionsBox->spaceSpinBox->setValue(number);
}

void ImportImageSeqDialog::importArbitrarySequence()
{
    QStringList files = getFilePaths();
    int number = getSpace();

    // Show a progress dialog, as this can take a while if you have lots of images.
    QProgressDialog progress(tr("Importing image sequence..."), tr("Abort"), 0, 100, mParent);
    hideQuestionMark(progress);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();

    int totalImagesToImport = files.count();
    int imagesImportedSoFar = 0;
    progress.setMaximum(totalImagesToImport);

    QString failedFiles;
    bool failedImport = false;
    for (const QString& strImgFile : files)
    {
        QString strImgFileLower = strImgFile.toLower();

        if (strImgFileLower.endsWith(".png") ||
            strImgFileLower.endsWith(".jpg") ||
            strImgFileLower.endsWith(".jpeg") ||
            strImgFileLower.endsWith(".bmp") ||
            strImgFileLower.endsWith(".tif") ||
            strImgFileLower.endsWith(".tiff"))
        {
            mEditor->importImage(strImgFile);

            imagesImportedSoFar++;
            progress.setValue(imagesImportedSoFar);
            QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);  // Required to make progress bar update

            if (progress.wasCanceled())
            {
                break;
            }
        }
        else
        {
            failedFiles += strImgFile + "\n";
            if (!failedImport)
            {
                failedImport = true;
            }
        }

        for (int i = 1; i < number; i++)
        {
            mEditor->scrubForward();
        }
    }

    if (failedImport)
    {
        QMessageBox::warning(mParent,
                             tr("Warning"),
                             tr("Unable to import") + failedFiles,
                             QMessageBox::Ok,
                             QMessageBox::Ok);
    }


    emit notifyAnimationLengthChanged();
    progress.close();
}

const QList<QPair<QString, int> > ImportImageSeqDialog::predefinedKeySetParams() const
{
    QString strFilePath = getFilePath();
    QList<QPair<QString, int> > fileSet;

    QRegularExpression pattern("^(.*?)(\\d+)([^\\d\\.]*\\.[^\\.]+)$");
    QString basename = QFileInfo(strFilePath).fileName();
    QRegularExpressionMatch match = pattern.match(basename);
    if (!match.hasMatch()) { return fileSet; }

    // local vars for testing file validity
    /*int dot = strFilePath.lastIndexOf(".");
    int slash = strFilePath.lastIndexOf("/");
    QString fName = strFilePath.mid(slash + 1);
    QString path = strFilePath.left(slash + 1);
    QString digit = strFilePath.mid(slash + 1, dot - slash - 1);*/

    // Find number of digits (min: 1, max: digit.length - 1)
    /*int digits = 0;
    for (int i = digit.length() - 1; i > 0; i--)
    {
        if (digit.at(i).isDigit())
        {
            digits++;
        }
        else
        {
            break;
        }
    }

    if (digits < 1)
    {
        return setParams;
    }

    digit = strFilePath.mid(dot - digits, digits);
    QString prefix = strFilePath.mid(slash + 1, dot - slash - digits - 1);
    QString suffix = strFilePath.mid(dot, strFilePath.length() - 1);*/

    QString prefix = match.captured(1);
    QString suffix = match.captured(3);

    QDir dir = strFilePath.left(strFilePath.lastIndexOf("/"));
    QStringList sList = dir.entryList(QDir::Files, QDir::Name);
    if (sList.isEmpty()) { return fileSet; }

    // List of files is not empty. Let's go find the relevant files
    QStringList finalList;
    for (int i = 0; i < sList.size(); i++)
    {
        QRegularExpressionMatch newMatch = pattern.match(sList[i]);
        if (newMatch.hasMatch() &&
            newMatch.captured(1) == prefix &&
            newMatch.captured(3) == suffix)
        {
            fileSet.append(QPair<QString,int>(dir.filePath(sList[i]), newMatch.captured(2).toInt()));
        }
    }
    return fileSet;
    /*
    if (finalList.isEmpty()) { return fileSet; }

    // List of relevant files is not empty. Let's validate them
    dot = finalList[0].lastIndexOf(".");

    QStringList absolutePaths;
    for (QString fileName : finalList) {
        absolutePaths << path + fileName;
    }

    setParams.dot = dot;
    setParams.digits = digits;
    setParams.filenames = finalList;
    setParams.folderPath = path;
    setParams.absolutePaths = absolutePaths;
    setParams.prefix = prefix;
    return setParams;*/
}

void ImportImageSeqDialog::importPredefinedSet()
{
    PredefinedKeySet keySet = generatePredefinedKeySet();

    // Show a progress dialog, as this can take a while if you have lots of images.
    QProgressDialog progress(tr("Importing images..."), tr("Abort"), 0, 100, mParent);
    hideQuestionMark(progress);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();

    int totalImagesToImport = keySet.size();
    int imagesImportedSoFar = 0;
    progress.setMaximum(totalImagesToImport);

    mEditor->layers()->createBitmapLayer(keySet.layerName());

    for (int i = 0; i < keySet.size(); i++)
    {
        const int& frameIndex = keySet.keyFrameIndexAt(i);
        const QString& filePath = keySet.filePathAt(i);

        mEditor->scrubTo(frameIndex);
        bool ok = mEditor->importImage(filePath);
        imagesImportedSoFar++;

        progress.setValue(imagesImportedSoFar);
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);  // Required to make progress bar update

        if (progress.wasCanceled())
        {
            break;
        }

        if (!ok) { return;}
    }

    emit notifyAnimationLengthChanged();
}

QStringList ImportImageSeqDialog::getFilePaths()
{
    return ImportExportDialog::getFilePaths();
}

Status ImportImageSeqDialog::validateKeySet(const PredefinedKeySet& keySet, const QStringList& filepaths)
{
    QString msg = "";
    QString failedPathsString;

    Status status = Status::OK;

    if (filepaths.isEmpty()) { status = Status::FAIL; }

    if (keySet.isEmpty())
    {
        status = Status::FAIL;
        failedPathsString = QLocale().createSeparatedList(filepaths);
    }

    if (status == Status::FAIL)
    {
        status.setTitle(tr("Invalid path"));
        status.setDescription(QString(tr("The following file did not meet the criteria: \n%1 \n\nRead the instructions and try again")).arg(failedPathsString));
    }

    return status;
}

Status ImportImageSeqDialog::validateFiles(const QStringList &filepaths)
{
    QString failedPathsString = "";

    Status status = Status::OK;

    if (filepaths.isEmpty()) { status = Status::FAIL; }

    for (int i = 0; i < filepaths.count(); i++)
    {
        QFileInfo file = filepaths.at(i);
        if (!file.exists())
            failedPathsString += filepaths.at(i) + "\n";
    }

    if (!failedPathsString.isEmpty())
    {
        status = Status::FAIL;
        status.setTitle(tr("Invalid path"));
        status.setDescription(QString(tr("The following file(-s) did not meet the criteria: \n%1")).arg(failedPathsString));
    }

    if (status == Status::OK)
    {
        getDialogButtonBox()->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(true);
    }
    return status;
}
