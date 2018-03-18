#ifndef RECOVERWINDOW_H
#define RECOVERWINDOW_H

#include <QMainWindow>
#include <storagedetector.h>
#include <processing.h>

class QFileDialog;
class ImagesList;
class QThreadPool;
class QProgressBar;
class ProgressDrawer;
class QSettings;


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

public:
    explicit RecoverWindow(QWidget *parent = 0);
    ~RecoverWindow();

private:
    Ui::RecoverWindow *ui;
public slots:
    void startScanning();
    void paramChanged();
    void storeDir(const QString& dir);
signals:
    void storeLog(const QString& message);
};

#endif // RECOVERWINDOW_H
