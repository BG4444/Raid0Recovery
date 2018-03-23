#ifndef PROCESSING_H
#define PROCESSING_H

#include <QRunnable>
#include <QObject>
#include <QEventLoop>
#include "signaturedefinterface.h"

class ImagesList;

class Processing :public QObject, public QRunnable
{
    Q_OBJECT

    ImagesList* imgs;
    const vDetectors& dets;
public:
    Processing(ImagesList* imgs, const vDetectors& dets);

    // QRunnable interface
public:
    void run();
signals:
    void storeLog(const QString&);
    void taskReleased();
    void onTaskRelease();
};

#endif // PROCESSING_H
