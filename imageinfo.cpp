#include "imageinfo.h"

void ImageInfo::setProgress(int value)
{
    progress = value;
    emit progressChanged();
}

int ImageInfo::getProgress() const
{
    return progress;
}

ImageInfo::ImageInfo(SignatureList *signatures,
                     storageSet::const_iterator storage,
                     int threadCount,
                     quint64 size,
                     uchar *base,
                     QObject *parent)
    :
      QObject(parent),
      progress(0),
      signatures(signatures),
      storage(storage),
      sem(new QSemaphore(threadCount)),
      nUsedAlgorithms(0),
      size(size),
      base(base)
{

}
