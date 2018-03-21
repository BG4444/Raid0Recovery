#ifndef IMAGEINFO_H
#define IMAGEINFO_H

#include <QObject>
#include <QVector>
#include <atomic>

#include "storagedetector.h"

class SignatureList;

class ImageInfo : public QObject
{
    Q_OBJECT   
    QVector<int> progress;
    std::unique_ptr<QSemaphore> sem;
public:
    SignatureList* signatures;
    storageSet::const_iterator storage;    
    std::atomic<size_t> nUsedAlgorithms;
    quint64 size;
    uchar* base;

    explicit ImageInfo(SignatureList* signatures,
                       storageSet::const_iterator storage,
                       int threadCount,
                       quint64 size,
                       uchar* base,
                       QObject *parent);



    const QVector<int>& getProgress() const;

    bool tryAcquire();

    void release();

signals:
    void progressChanged();
public slots:
   void setProgress(int nAlg,int value);
   void setThreadCount(int count);
};

#endif // IMAGEINFO_H
