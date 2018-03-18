#ifndef IMAGEINFO_H
#define IMAGEINFO_H

#include <QObject>

#include "storagedetector.h"

class SignatureList;

class ImageInfo : public QObject
{
    Q_OBJECT   
    int progress;
public:
    SignatureList* signatures;
    storageSet::const_iterator storage;
    std::unique_ptr<QSemaphore> sem;
    std::atomic<size_t> nUsedAlgorithms;
    quint64 size;
    uchar* base;

    explicit ImageInfo(SignatureList* signatures,
                       storageSet::const_iterator storage,
                       int threadCount,
                       quint64 size,
                       uchar* base,
                       QObject *parent);



    int getProgress() const;

signals:
    void progressChanged();
public slots:
   void setProgress(int value);
};

#endif // IMAGEINFO_H
