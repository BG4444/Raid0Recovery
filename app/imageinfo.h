#ifndef IMAGEINFO_H
#define IMAGEINFO_H

#include <QObject>
#include <QVector>
#include <atomic>
#include <signaturelist.h>
#include "storagedetector.h"


class ImageInfo : public QObject
{
    Q_OBJECT   
    QVector< int > progress;
    std::unique_ptr<QSemaphore> sem;
public:
    std::unique_ptr<SignatureList> signatures;
    storageSet::const_iterator storage;    
    std::atomic<size_t> nUsedAlgorithms;    

    explicit ImageInfo(SignatureList* signatures,
                       storageSet::const_iterator storage,
                       int threadCount,
                       QObject *parent);


    const QVector<int> &getProgress() const;

    bool tryAcquire();

    void release();

signals:
    void progressChanged();
public slots:
   void setProgress(int nAlg,int value);
   void setThreadCount(int count);
};

#endif // IMAGEINFO_H
