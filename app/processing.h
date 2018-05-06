#ifndef PROCESSING_H
#define PROCESSING_H

#include <QRunnable>
#include <QObject>
#include <QEventLoop>
#include "signaturedefinterface.h"
#include "vdetectors.h"
#include <functional>

class ImagesList;
class ImageInfo;
class QFile;

using Applicator=std::function<void(ImageInfo*,QFile*,bool&, bool&)>;

class Processing :public QObject, public QRunnable
{
    Q_OBJECT
    ImagesList* imgs;        
    Applicator apply;
public:
    Processing(ImagesList* imgs, Applicator  apply);
    void run();

signals:
    void storeLog(const QString&);
    void taskReleased();
    void onTaskRelease();
};

#endif // PROCESSING_H
