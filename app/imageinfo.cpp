#include "imageinfo.h"

void ImageInfo::setProgress(int nAlg, int value)
{
    if(progress[nAlg] < value)
    {
        progress[nAlg] = value;
        emit progressChanged();
    }
}

void ImageInfo::setThreadCount(int count)
{
    sem.reset(new QSemaphore(count));
    progress.resize(signatures->vDet.size());
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
                     QObject *parent)
    :
      QObject(parent),
      progress(signatures->vDet.size() , 0),
      sem(new QSemaphore(threadCount)),
      signatures(signatures),
      storage(storage),
      nUsedAlgorithms(0)
{

}

