#ifndef STORAGEDETECTOR_H
#define STORAGEDETECTOR_H

#include <map>
#include <QStorageInfo>
#include <algorithm>
#include <QSemaphore>
#include <memory>

struct cmpStorages
{
    bool operator () (const QStorageInfo& a,const QStorageInfo& b);
};


using PSemaphore=std::unique_ptr<QSemaphore>;

using storageSet=std::map<QStorageInfo,PSemaphore,cmpStorages>;

class StorageDetector:public QObject,public storageSet
{    
    Q_OBJECT
public:
    StorageDetector(const int threadCount=1);
    storageSet::const_iterator detect(const QString& fileName) const;
public slots:
    void setThreadCount(const int count);
};

#endif // STORAGEDETECTOR_H
