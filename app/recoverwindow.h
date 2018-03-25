#ifndef RECOVERWINDOW_H
#define RECOVERWINDOW_H

#include "fileglue.h"

#include <QMainWindow>
#include <storagedetector.h>
#include <processing.h>

class QFileDialog;
class ImagesList;
class QThreadPool;
class QProgressBar;
class ProgressDrawer;
class QSettings;
class QSpinBox;

namespace Ui {
class RecoverWindow;
}

class RecoverWindow : public QMainWindow
{
    Q_OBJECT
    QSettings* cfg;
    QFileDialog* dlg;
    ImagesList* imgs;
    QThreadPool* tpool;
    QProgressBar* pBar;
    ProgressDrawer* drw;
    StorageDetector sDetect;
    vDetectors signatureDetectors;

    std::unique_ptr<FileGlue> glue;

    void loadSpinBoxSetting(QSpinBox* box,const int defValue);

    void loadPlugins();

    std::atomic<bool> stopper;

public:
    explicit RecoverWindow(QWidget *parent = 0);
    ~RecoverWindow();

private:
    Ui::RecoverWindow *ui;
    void doScan();
    void doStop();
public slots:
    void startScanning(const bool toggle);
    void paramChanged();
    void storeDir(const QString& dir);
    void updateProgressMaximum();
    void store();
    void load();
    void tableClicked(const QModelIndex& idx);
    void build();
    void roll();
    void fix();
signals:
    void doBuild();
    void storeLog(const QString& message);
};

#endif // RECOVERWINDOW_H
