#ifndef PROCESSING_H
#define PROCESSING_H

#include <QRunnable>
#include <QObject>
#include <QEventLoop>
#include "signaturedefinterface.h"
#include "vdetectors.h"

class ImagesList;

class Processing :public QObject, public QRunnable
{
    Q_OBJECT

    ImagesList* imgs;
    const vDetectors& dets;
    const std::atomic<bool> &stopper;
public:
    Processing(ImagesList* imgs, const vDetectors& dets, const std::atomic<bool> &stopper);

    // QRunnable interface
public:
    void run();
signals:
    void storeLog(const QString&);
    void taskReleased();
    void onTaskRelease();
};

#endif // PROCESSING_H
