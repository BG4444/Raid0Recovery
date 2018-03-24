#include "imageinfo.h"

void ImageInfo::setProgress(int nAlg, int value)
{
    progress[nAlg] = value;
    emit progressChanged();
}

void ImageInfo::setThreadCount(int count)
{
    sem.reset(new QSemaphore(count));
    progress.resize(count);
    std::fill(progress.begin(),progress.end(),0);
    emit progressChanged();
}

const QVector<int> &ImageInfo::getProgress() const
{
    return progress;
}

bool ImageInfo::tryAcquire()
{
    return sem->tryAcquire();
}

void ImageInfo::release()
{
    sem->release();
}

ImageInfo::ImageInfo(SignatureList *signatures,
                     storageSet::const_iterator storage,
                     int threadCount,
                     quint64 size,
                     uchar *base,
                     QObject *parent)
    :
      QObject(parent),
      progress(threadCount,0),
      sem(new QSemaphore(threadCount)),
      signatures(signatures),
      storage(storage),
      nUsedAlgorithms(0),
      size(size),
      base(base)
{

}

