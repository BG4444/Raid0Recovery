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


namespace Ui {
class RecoverWindow;
}

class RecoverWindow : public QMainWindow
{
    Q_OBJECT
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
signals:
    void storeLog(const QString& message);
};

#endif // RECOVERWINDOW_H
